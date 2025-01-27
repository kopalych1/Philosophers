/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akostian <akostian@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 17:18:43 by akostian          #+#    #+#             */
/*   Updated: 2025/01/27 12:00:25 by akostian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philosophers.h"

#define NUM_OF_PHILO 50
#define EATING_TIME_MS 500
#define SLEEPING_TIME_MS 1000
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

void	*philosopher_thread(void *arg)
{
	t_ph_thread_args	philo_args;
	unsigned int		eaten_times;
	// unsigned char	is_odd;
	// time_t			start;
	// unsigned int		flag;

	philo_args = *(t_ph_thread_args *)arg;

	// is_odd = philo_args.philo_n % 2 == 0;
	// flag = 1;

	// if (NUM_OF_PHILO % 2 == 0)
	// 	start = is_odd * EATING_TIME_MS;
	// else
	// 	start = ((philo_args.philo_n == flag) * 2 + is_odd) * EATING_TIME_MS;


	// pthread_mutex_lock(philo_args.print_mutex);
	// printf(CLR_PHILO_N": ", philo_args.philo_n);
	// for (size_t i = 0; i <= DESIRED_EATEN; i++)
	// {
	// 	if (NUM_OF_PHILO % 2 == 0)
	// 		printf("%li-", (time_t)(start + (EATING_TIME_MS * i) + (SLEEPING_TIME_MS * i) + ft_imax(EATING_TIME_MS - SLEEPING_TIME_MS, 0) * i));
	// 	else
	// 		printf("%li-", (time_t)(start + (EATING_TIME_MS * i) + (SLEEPING_TIME_MS * i) + ft_imax(EATING_TIME_MS - SLEEPING_TIME_MS, 0) * i) + (philo_args.philo_n == flag) * EATING_TIME_MS);
	// 	flag++;
	// }
	// printf("\n");
	// pthread_mutex_unlock(philo_args.print_mutex);

	// return (0);

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
	// t_sim_settings		sim_settings;

	// if (parse(argc, argv, &sim_settings))
	// 	return (printf(INCORRECT_ARGS_ERROR_MESSAGE), 1);
	
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
	return (0);
}
