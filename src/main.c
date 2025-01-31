/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akostian <akostian@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 17:18:43 by akostian          #+#    #+#             */
/*   Updated: 2025/01/31 11:37:52 by akostian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philosophers.h"

time_t	time_diff(time_t tv0)
{
	return (get_time_ms() - tv0);
}

void	philo_wait(t_ph_data *philo_data, unsigned int wait_time)
{
	time_t	wake_up;

	wake_up = time_diff(philo_data->start_time) + wait_time;
	while (1)
	{
		if (time_diff(philo_data->start_time) > wake_up)
			return ;
		usleep(20);
	}
}

void	philo_print(t_ph_data *philo_data, char *action)
{
	pthread_mutex_lock(philo_data->print_mutex);
	printf("%-8li", time_diff(philo_data->start_time));
	printf(BYEL "%-4d" CRESET, philo_data->philo_n);
	printf("%s", action);
	pthread_mutex_unlock(philo_data->print_mutex);
}

void	*philosopher_thread(void *arg)
{
	t_ph_data		philo_data;
	unsigned int	eaten_times;

	philo_data = *(t_ph_data *)arg;
	eaten_times = 0;
	while ((eaten_times < philo_data.sim->ph_eat_count)
		|| !philo_data.sim->ph_eat_count)
	{
		pthread_mutex_lock(philo_data.left_fork);
		philo_print(&philo_data, BMAG"has taken a fork\n"CRESET);
		pthread_mutex_lock(philo_data.right_fork);
		philo_print(&philo_data, BMAG"has taken a fork\n"CRESET);
		philo_print(&philo_data, BRED"is eating\n"CRESET);
		philo_wait(&philo_data, philo_data.sim->time_to_eat);
		pthread_mutex_unlock(philo_data.left_fork);
		pthread_mutex_unlock(philo_data.right_fork);
		eaten_times++;
		philo_print(&philo_data, BBLU"is sleeping\n"CRESET);
		philo_wait(&philo_data, philo_data.sim->time_to_slp);
		philo_print(&philo_data, BGRN"is thinking\n"CRESET);
	}
	return (0);
}

unsigned char	init_data(
	t_sim_settings *sim,
	pthread_t **threads,
	t_ph_data **philo_data,
	pthread_mutex_t *print_mutex
)
{
	unsigned int	i;

	*threads = ft_calloc(sim->philo_n, sizeof(pthread_t));
	*philo_data = ft_calloc(sim->philo_n, sizeof(t_ph_data));
	sim->forks = ft_calloc(sim->philo_n, sizeof(pthread_mutex_t));
	if (!*threads || !*philo_data || !sim->forks)
		return (free(*threads), free(*philo_data), free(sim->forks), 1);
	if (pthread_mutex_init(print_mutex, NULL))
		return (free(*threads), free(*philo_data), free(sim->forks), 1);
	sim->print_mutex = print_mutex;
	i = -1;
	while (++i < sim->philo_n)
	{
		if (pthread_mutex_init(&(sim->forks[i]), NULL))
			return (free(*threads), free(*philo_data), free(sim->forks), 1);
		(*philo_data)[i].philo_n = i + 1;
		(*philo_data)[i].print_mutex = sim->print_mutex;
		(*philo_data)[i].sim = sim;
		if (i == 0)
			(*philo_data)[i].right_fork = &(sim->forks[sim->philo_n - 1]);
		else
			(*philo_data)[i].right_fork = &(sim->forks[i - 1]);
		(*philo_data)[i].left_fork = &(sim->forks[i]);
	}
	return (0);
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
			return (1);
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
			return (1);
		i += 2;
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
		return (printf(NO_MEMORY_ERROR_MESSAGE), ENOMEM);
	for (unsigned int i = 0; i < sim_settings.philo_n; i++)
		pthread_join(threads[i], NULL);
	printf("ms All threads are done.\n");
	return (free(threads), free(philosopher_data), free(sim_settings.forks), 0);
}
