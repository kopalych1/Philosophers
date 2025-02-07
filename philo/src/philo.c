/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akostian <akostian@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 00:36:59 by akostian          #+#    #+#             */
/*   Updated: 2025/02/07 01:11:58 by akostian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philosophers.h"

static void	philo_wait(t_ph_data *philo_data, unsigned int wait_time)
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

static int	philo_sleep_think(t_ph_data *philo_data)
{
	philo_print(philo_data, PHILO_SLEEPING_MSG);
	philo_wait(philo_data, philo_data->sim->time_to_slp);
	if (philo_data->sim->is_dead)
		return (1);
	philo_print(philo_data, PHILO_THINKING_MSG);
	return (0);
}

static int	philo_eat(t_ph_data *philo_data, unsigned int *eaten_times)
{
	if (philo_data->sim->is_dead)
		return (0);
	pthread_mutex_lock(philo_data->left_fork);
	if (philo_data->sim->is_dead)
		return (pthread_mutex_unlock(philo_data->left_fork), 1);
	philo_print(philo_data, PHILO_TAKING_MSG);
	pthread_mutex_lock(philo_data->right_fork);
	if (philo_data->sim->is_dead)
	{
		pthread_mutex_unlock(philo_data->left_fork);
		pthread_mutex_unlock(philo_data->right_fork);
		return (1);
	}
	philo_print(philo_data, PHILO_TAKING_MSG);
	philo_print(philo_data, PHILO_EATING_MSG);
	philo_wait(philo_data, philo_data->sim->time_to_eat);
	pthread_mutex_unlock(philo_data->left_fork);
	pthread_mutex_unlock(philo_data->right_fork);
	if (philo_data->sim->is_dead)
		return (1);
	(*eaten_times)++;
	return (0);
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
		if (philo_eat(&philo_data, &eaten_times))
			return (0);
		if ((eaten_times >= philo_data.sim->ph_eat_count)
			&& philo_data.sim->ph_eat_count)
			philo_data.sim->eat_enough[philo_data.philo_n - 1] = 1;
		philo_data.sim->last_meals[philo_data.philo_n - 1]
			= time_diff(philo_data.start_time);
		if (philo_sleep_think(&philo_data))
			return (0);
	}
	return (0);
}
