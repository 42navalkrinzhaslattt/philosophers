/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 07:38:00 by nam-vu            #+#    #+#             */
/*   Updated: 2024/04/11 07:38:00 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	action_log(t_philo *philo, int action, long long timestamp)
{
	pthread_mutex_lock(&philo->data->death);
	pthread_mutex_lock(&philo->data->write);
	if (!philo->funeral && philo->group->index == 1)
		printf("\t");
	if (!philo->funeral && philo->group->index == 2)
		printf("\t\t");
	timestamp = -1;
	if (!philo->funeral && timestamp == -1)
		printf("%lli %d ", ft_gettime(philo->start_time), philo->index);
	else if (!philo->funeral && timestamp >= 0)
		printf("%lli %d ", timestamp, philo->index);
	if (action == FORK && !philo->funeral)
		printf("has taken a fork\n");
	else if (action == EAT && !philo->funeral)
		printf("is eating\n");
	else if (action == SLEEP && !philo->funeral)
		printf("is sleeping\n");
	else if (action == THINK && !philo->funeral)
		printf("is thinking\n");
	else if (action == DIE && !philo->funeral)
		printf("died\n");
	pthread_mutex_unlock(&philo->data->write);
	pthread_mutex_unlock(&philo->data->death);
}

int	print_error(int status, t_data *data)
{
	write(2, "philo: ", 7);
	if (status == INV_ARG_STATUS)
		write(2, INV_ARG_MSG, 18);
	if (status == ERR_MALLOC_STATUS)
		write(2, ERR_MALLOC_MSG, 13);
	if (status != INV_ARG_STATUS)
		free_all(data);
	return (0);
}

long	ft_atoi(const char *nptr)
{
	int		i;
	long	res;

	i = 0;
	res = 0;
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		res = res * 10 + nptr[i] - 48;
		if (res > INT32_MAX || res < INT32_MIN)
			return (-2);
		i++;
	}
	if (nptr[i] != 0)
		return (-2);
	return (res);
}

long long	ft_gettime(long long start)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000 - start);
}

void	free_all(t_data *data)
{
	int	i;

	if (data->nb_philo)
	{
		i = -1;
		while (++i < data->nb_group)
			pthread_mutex_destroy(&data->group[i].count);
		free(data->group);
		i = -1;
		while (++i < data->nb_philo)
		{
			pthread_mutex_destroy(&data->forks[i]);
			pthread_mutex_destroy(&data->philo[i].meal);
		}
		free(data->philo);
		free(data->forks);
	}
	pthread_mutex_destroy(&data->write);
	pthread_mutex_destroy(&data->death);
	pthread_mutex_destroy(&data->flag);
}
