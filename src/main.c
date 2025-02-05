/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akostian <akostian@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 17:18:43 by akostian          #+#    #+#             */
/*   Updated: 2025/02/05 14:00:47 by akostian         ###   ########.fr       */
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
	printf(PHILO_N, philo_data->philo_n);
	printf("%s", action);
	pthread_mutex_unlock(philo_data->print_mutex);
}

char	**generate_extra_wait_map(unsigned short int n_of_philos)
{
	const short unsigned int	jump = (n_of_philos - 1) / 2;
	char						**ret;
	short int					i;

	ret = ft_calloc(n_of_philos, sizeof(char *));
	if (!ret)
		return (NULL);
	i = -1;
	while (++i < n_of_philos)
	{
		ret[i] = ft_calloc(jump, sizeof(char));
		if (!ret[i])
			return (NULL);
		if (i == 0)
			ret[i][0] = 1;
		else
			ret[i][((n_of_philos - 1) / 2 - 1) - (i - 1) / 2] = 1;
	}
	return (ret);
}

time_t	next_meal(
		t_sim_settings *sim,
		unsigned short int philo_n,
		unsigned int eaten_times,
		char *map
)
{
	unsigned char		is_odd;
	time_t				ret;
	time_t				start;
	unsigned short int	jump;
	(void)sim;

	is_odd = philo_n % 2 == 0;
	start = is_odd * sim->time_to_eat;
	if (sim->philo_n % 2 == 0)
		ret = start + (sim->time_to_eat * eaten_times) + (sim->time_to_slp * eaten_times)
			+ ft_imax(sim->time_to_eat - sim->time_to_slp, 0) * eaten_times;
	else
	{
		jump = (sim->philo_n - 1) / 2;
		ret =
			start + (sim->time_to_eat * eaten_times) + (sim->time_to_slp * eaten_times)
			+ (ft_imax(sim->time_to_eat - sim->time_to_slp, 0) * eaten_times)
			+ ((map[eaten_times % jump] && philo_n && (philo_n - 1 || eaten_times)) * sim->time_to_eat)
			+ eaten_times / jump * sim->time_to_eat;
	}
	return (ret);
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
		philo_print(&philo_data, PHILO_TAKING_MSG);
		pthread_mutex_lock(philo_data.right_fork);
		philo_print(&philo_data, PHILO_TAKING_MSG);
		philo_print(&philo_data, PHILO_EATING_MSG);
		philo_wait(&philo_data, philo_data.sim->time_to_eat);
		pthread_mutex_unlock(philo_data.left_fork);
		pthread_mutex_unlock(philo_data.right_fork);
		eaten_times++;
		philo_print(&philo_data, PHILO_SLEEPING_MSG);
		philo_wait(&philo_data, philo_data.sim->time_to_slp);
		philo_print(&philo_data, PHILO_THINKING_MSG);
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
	// printf("ms All threads are done.\n");
	return (free(threads), free(philosopher_data), free(sim_settings.forks), 0);
}
