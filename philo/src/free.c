/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akostian <akostian@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 05:24:48 by akostian          #+#    #+#             */
/*   Updated: 2025/02/07 05:25:01 by akostian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philosophers.h"

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
