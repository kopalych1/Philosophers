/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akostian <akostian@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 17:18:43 by akostian          #+#    #+#             */
/*   Updated: 2025/02/07 00:19:31 by akostian         ###   ########.fr       */
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
	while (!philo_data->sim->is_dead)
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
	printf(PHILO_N, philo_data->philo_n);
	printf("%s", action);
	pthread_mutex_unlock(philo_data->print_mutex);
}

void	*philosopher_thread(void *arg)
{
	t_ph_data		philo_data;
	unsigned int	eaten_times;

	philo_data = *(t_ph_data *)arg;
	eaten_times = 0;
	philo_data.sim->last_meals[philo_data.philo_n - 1]
		= time_diff(philo_data.start_time);
	if (philo_data.sim->philo_n == 1)
		philo_wait(&philo_data, philo_data.sim->time_to_die + 10);
	while (1)
	{
		if (philo_data.sim->is_dead)
			return (0);
		pthread_mutex_lock(philo_data.left_fork);
		if (philo_data.sim->is_dead)
			return (pthread_mutex_unlock(philo_data.left_fork), NULL);
		philo_print(&philo_data, PHILO_TAKING_MSG);
		pthread_mutex_lock(philo_data.right_fork);
		if (philo_data.sim->is_dead)
		{
			pthread_mutex_unlock(philo_data.left_fork);
			pthread_mutex_unlock(philo_data.right_fork);
			return (NULL);
		}
		philo_print(&philo_data, PHILO_TAKING_MSG);
		philo_print(&philo_data, PHILO_EATING_MSG);
		philo_wait(&philo_data, philo_data.sim->time_to_eat);
		pthread_mutex_unlock(philo_data.left_fork);
		pthread_mutex_unlock(philo_data.right_fork);
		if (philo_data.sim->is_dead)
			return (0);
		eaten_times++;
		if ((eaten_times >= philo_data.sim->ph_eat_count)
			&& philo_data.sim->ph_eat_count)
			philo_data.sim->eat_enough[philo_data.philo_n - 1] = 1;
		philo_data.sim->last_meals[philo_data.philo_n - 1]
			= time_diff(philo_data.start_time);
		philo_print(&philo_data, PHILO_SLEEPING_MSG);
		philo_wait(&philo_data, philo_data.sim->time_to_slp);
		if (philo_data.sim->is_dead)
			return (0);
		philo_print(&philo_data, PHILO_THINKING_MSG);
	}
	return (0);
}

void	free_data(
	t_sim_settings *sim,
	pthread_t **threads
)
{
	free(*threads);
	free(sim->philo_data);
	free(sim->forks);
	free(sim->last_meals);
	free(sim->eat_enough);
}

int	alloc_data(
	t_sim_settings *sim,
	pthread_t **threads,
	t_ph_data **philo_data
)
{
	*threads = ft_calloc(sim->philo_n + 1, sizeof(pthread_t));
	*philo_data = ft_calloc(sim->philo_n, sizeof(t_ph_data));
	sim->forks = ft_calloc(sim->philo_n, sizeof(pthread_mutex_t));
	sim->last_meals = ft_calloc(sim->philo_n, sizeof(time_t));
	sim->eat_enough = ft_calloc(sim->philo_n, sizeof(unsigned char));
	sim->philo_data = *philo_data;
	if (!*threads || !*philo_data
		|| !sim->forks || !sim->last_meals || !sim->eat_enough)
	{
		free_data(sim, threads);
		return (ENOMEM);
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

	if (alloc_data(sim, threads, philo_data))
		return (ENOMEM);
	if (pthread_mutex_init(print_mutex, NULL))
		return (free_data(sim, threads), 1);
	sim->print_mutex = print_mutex;
	i = -1;
	while (++i < sim->philo_n)
	{
		if (pthread_mutex_init(&(sim->forks[i]), NULL))
			return (free_data(sim, threads), 1);
		(*philo_data)[i].philo_n = i + 1;
		(*philo_data)[i].print_mutex = sim->print_mutex;
		(*philo_data)[i].sim = sim;
		if (i == 0)
			(*philo_data)[i].right_fork = &(sim->forks[sim->philo_n - 1]);
		else
			(*philo_data)[i].right_fork = &(sim->forks[i - 1]);
		(*philo_data)[i].left_fork = &(sim->forks[i]);
	}
	return (sim->is_dead = 0, sim->philo_data = *philo_data, 0);
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
	unsigned int		i;

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
		return (free_data(&sim_settings, &threads), 1);
	i = 0;
	while (i < sim_settings.philo_n + 1)
		pthread_join(threads[i++], NULL);
	return (free_data(&sim_settings, &threads), 0);
}
