/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akostian <akostian@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 17:18:43 by akostian          #+#    #+#             */
/*   Updated: 2025/01/28 11:16:09 by akostian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philosophers.h"

#define NUM_OF_PHILO 7
#define EATING_TIME_MS 500
#define SLEEPING_TIME_MS 250
#define DESIRED_EATEN 4UL

#define CLR_PHILO_N	HYEL"%-3d"CRESET
#define CLR_FORK_N	HMAG"%d"CRESET

void	sleep_until(time_t	wake_up)
{
	while (get_time_ms() <= wake_up)
		usleep(50);
}

void	smart_sleep(
		unsigned short int philo_n,
		time_t start_time,
		time_t sleep_time
)
{
	time_t			wake_up_after;
	// static int		philo_rest[NUM_OF_PHILO];

	wake_up_after = (get_time_ms() - start_time + sleep_time) / 10 * 10;

	(void)philo_n;
	// if ((philo_rest[philo_n - 1] + (sleep_time % 10)) < 10)
	// 	philo_rest[philo_n - 1] = (philo_rest[philo_n - 1] + (sleep_time % 10));
	// else
	// 	philo_rest[philo_n - 1] = (philo_rest[philo_n - 1] + (sleep_time % 10));

	// printf("--------------------\n");
	// printf("philo_n: "CLR_PHILO_N"\n", philo_n);
	// printf("rest: %d\n", philo_rest[philo_n - 1]);
	// printf("waiting until: %ld\n", wake_up_after + philo_rest[philo_n - 1]);
	// printf("--------------------\n");

	while (get_time_ms() < (wake_up_after + start_time /* + philo_rest[philo_n - 1] */))
		usleep(5);
}

void	print_time(time_t t0)
{
	printf("%-8li", get_time_ms() - t0);
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
	start = is_odd * EATING_TIME_MS;
	if (NUM_OF_PHILO % 2 == 0)
		ret = start + (EATING_TIME_MS * eaten_times) + (SLEEPING_TIME_MS * eaten_times)
			+ ft_imax(EATING_TIME_MS - SLEEPING_TIME_MS, 0) * eaten_times;
	else
	{
		jump = (NUM_OF_PHILO - 1) / 2;
		ret =
			start + (EATING_TIME_MS * eaten_times) + (SLEEPING_TIME_MS * eaten_times)
			+ (ft_imax(EATING_TIME_MS - SLEEPING_TIME_MS, 0) * eaten_times)
			+ ((map[eaten_times % jump] && philo_n && (philo_n - 1 || eaten_times)) * EATING_TIME_MS)
			+ eaten_times / jump * EATING_TIME_MS;
	}
	return (ret);
}

void	*philosopher_thread(void *arg)
{
	t_ph_thread_args	philo_args;
	unsigned int		eaten_times;
	time_t				next;

	philo_args = *(t_ph_thread_args *)arg;

	pthread_mutex_lock(philo_args.print_mutex);
	printf(CLR_PHILO_N": ", philo_args.philo_n);
	for (size_t i = 0; i < 10; i++)
	{
		next = next_meal(philo_args.sim, philo_args.philo_n, i, philo_args.map);
		printf("%ld-",
			next
		);
	}
	printf("\n");
	pthread_mutex_unlock(philo_args.print_mutex);

	return (0);

	eaten_times = 0;
	while (eaten_times < DESIRED_EATEN)
	{
		while (1)
		{
			pthread_mutex_lock(&(philo_args.left_fork->mutex));
			pthread_mutex_lock(&(philo_args.right_fork->mutex));

			if (!philo_args.left_fork->is_taken && !philo_args.right_fork->is_taken)
			{
				philo_args.left_fork->is_taken = 1;
				philo_args.right_fork->is_taken = 1;
				pthread_mutex_unlock(&(philo_args.left_fork->mutex));
				pthread_mutex_unlock(&(philo_args.right_fork->mutex));
				break;
			}

			pthread_mutex_unlock(&(philo_args.left_fork->mutex));
			pthread_mutex_unlock(&(philo_args.right_fork->mutex));
			usleep(20);
		}

		pthread_mutex_lock(philo_args.print_mutex);
		print_time(philo_args.start_time);
		printf(CLR_PHILO_N" is eating\n", philo_args.philo_n);
		pthread_mutex_unlock(philo_args.print_mutex);

		smart_sleep(philo_args.philo_n, philo_args.start_time, EATING_TIME_MS);

		pthread_mutex_lock(&(philo_args.left_fork->mutex));
		pthread_mutex_lock(&(philo_args.right_fork->mutex));
		philo_args.left_fork->is_taken = 0;
		philo_args.right_fork->is_taken = 0;
		pthread_mutex_unlock(&(philo_args.left_fork->mutex));
		pthread_mutex_unlock(&(philo_args.right_fork->mutex));

		eaten_times++;

		pthread_mutex_lock(philo_args.print_mutex);
		print_time(philo_args.start_time);
		printf(CLR_PHILO_N" is sleeping\n", philo_args.philo_n);
		pthread_mutex_unlock(philo_args.print_mutex);

		smart_sleep(philo_args.philo_n, philo_args.start_time, SLEEPING_TIME_MS);

		pthread_mutex_lock(philo_args.print_mutex);
		print_time(philo_args.start_time);
		printf(CLR_PHILO_N" is thinking\n", philo_args.philo_n);
		pthread_mutex_unlock(philo_args.print_mutex);
	}
	return (0);
}



#include <stdlib.h>

int main(/* int argc, char **argv */)
{
	pthread_t			threads[NUM_OF_PHILO];
	t_ph_thread_args	philosopher_args[NUM_OF_PHILO];
	t_fork				forks[NUM_OF_PHILO];
	pthread_mutex_t		print_mutex;
	time_t				start_time;
	char				**map;
	// t_sim_settings		sim_settings;

	// if (parse(argc, argv, &sim_settings))
	// 	return (printf(INCORRECT_ARGS_ERROR_MESSAGE), 1);
	
	if (NUM_OF_PHILO % 2)
		map = generate_extra_wait_map(NUM_OF_PHILO);

	start_time = get_time_ms();

	for (int i = 0; i < NUM_OF_PHILO; i++)
	{
		forks[i].fork_n = i;
		forks[i].is_taken = 0;
		pthread_mutex_init(&(forks[i].mutex), NULL);
	}
	pthread_mutex_init(&print_mutex, NULL);
	for (int i = 0; i < NUM_OF_PHILO; i++) {
		philosopher_args[i].start_time = start_time;
		philosopher_args[i].print_mutex = &print_mutex;
		philosopher_args[i].philo_n = i + 1;
		if (NUM_OF_PHILO % 2)
			philosopher_args[i].map = map[i];
		if (!i)
			philosopher_args[i].right_fork = &(forks[NUM_OF_PHILO - 1]);
		else
			philosopher_args[i].right_fork = &(forks[i - 1]);
		philosopher_args[i].left_fork = &(forks[i]);
		if (pthread_create(&threads[i], NULL, philosopher_thread, &philosopher_args[i]) != 0){
			fprintf(stderr, "Error creating thread %d\n", i + 1);
			exit(EXIT_FAILURE);
		}
	}
	for (int i = 0; i < NUM_OF_PHILO; i++)
		pthread_join(threads[i], NULL);
	printf("ms All threads are done.\n");

	if (NUM_OF_PHILO % 2)
	{
		for (size_t i = 0; i < NUM_OF_PHILO; i++)
			free(map[i]);
		free(map);
	}
	return (0);
}
