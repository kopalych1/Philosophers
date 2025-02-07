/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akostian <akostian@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 17:18:43 by akostian          #+#    #+#             */
/*   Updated: 2025/02/07 05:28:52 by akostian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philosophers.h"

void	cancel_all_philos(
	t_sim_settings *sim,
	pthread_t *threads
)
{
	unsigned int	i;

	i = 0;
	while (i < sim->philo_n + 1)
		pthread_join(threads[i++], NULL);
}

void	cancel_n_philos(
	pthread_t *threads,
	t_ph_data *philosopher_data,
	unsigned int i
)
{
	unsigned int	j;

	philosopher_data->sim->is_dead = 1;
	j = 0;
	while (j < philosopher_data->sim->philo_n && j != i)
	{
		printf("canceling: %d\n", j);
		pthread_join(threads[j], NULL);
		j += 2;
	}
	if (j == i)
		return ;
	j = 1;
	while (j < philosopher_data->sim->philo_n && j != i)
	{
		printf("canceling: %d\n", j);
		pthread_join(threads[j], NULL);
		j += 2;
	}
}

unsigned char	create_philos(
	t_sim_settings sim,
	pthread_t *threads,
	t_ph_data *philosopher_data
)
{
	time_t				start_time;
	unsigned int		i;

	start_time = get_time_ms();
	i = 0;
	while (i < sim.philo_n)
	{
		philosopher_data[i].start_time = start_time;
		if (pthread_create(&threads[i], NULL,
				philosopher_thread, &philosopher_data[i]) != 0)
			return (cancel_n_philos(threads, philosopher_data, i), 1);
		i += 2;
	}
	usleep(1000);
	start_time = get_time_ms();
	i = 1;
	while (i < sim.philo_n)
	{
		philosopher_data[i].start_time = start_time;
		if (pthread_create(&threads[i], NULL,
				philosopher_thread, &philosopher_data[i]) != 0)
			return (cancel_n_philos(threads, philosopher_data, i), 1);
		i += 2;
	}
	return (0);
}

void	*reaper_thread(void *arg)
{
	t_sim_settings	*sim;
	unsigned int	i;
	unsigned int	j;

	sim = (t_sim_settings *)arg;
	while (1)
	{
		i = -1;
		while (++i < sim->philo_n)
		{
			j = -1;
			while (++j < sim->philo_n)
				if (!sim->eat_enough[j])
					break ;
			if (j == sim->philo_n)
				return (sim->is_dead = 1, NULL);
			if ((time_diff(sim->philo_data[i].start_time) - sim->last_meals[i])
				> sim->time_to_die)
				return (philo_print(&(sim->philo_data[i]), PHILO_DIED_MSG),
					sim->is_dead = 1, NULL);
		}
		usleep(500);
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_sim_settings		sim_settings;
	pthread_t			*threads;
	t_ph_data			*philosopher_data;
	pthread_mutex_t		print_mutex;

	if (parse(argc, argv, &sim_settings))
		return (printf(INCORRECT_ARGS_ERROR_MESSAGE), 1);
	if (init_data(&sim_settings, &threads, &philosopher_data, &print_mutex))
		return (printf(NO_MEMORY_ERROR_MESSAGE), ENOMEM);
	if (create_philos(sim_settings, threads, philosopher_data))
		return (free_data(&sim_settings, &threads),
			printf(NO_MEMORY_ERROR_MESSAGE), ENOMEM);
	usleep(2000);
	if (pthread_create(&threads[sim_settings.philo_n], NULL,
			reaper_thread, &sim_settings) != 0)
	{
		sim_settings.is_dead = 1;
		cancel_all_philos(&sim_settings, threads);
		return (free_data(&sim_settings, &threads),
			printf(NO_MEMORY_ERROR_MESSAGE), ENOMEM);
	}
	cancel_all_philos(&sim_settings, threads);
	return (free_data(&sim_settings, &threads), 0);
}
