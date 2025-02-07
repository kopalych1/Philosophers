/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akostian <akostian@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 00:33:19 by akostian          #+#    #+#             */
/*   Updated: 2025/02/07 00:50:00 by akostian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philosophers.h"

static int	alloc_data(
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
