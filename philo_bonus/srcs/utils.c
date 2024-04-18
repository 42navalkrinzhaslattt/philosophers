/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/14 10:14:17 by nam-vu            #+#    #+#             */
/*   Updated: 2024/04/14 10:14:17 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	free_all(t_data *data)
{
	int	i;

	if (data->nb_philo)
	{
		i = -1;
		while (++i < data->nb_group)
		{
			sem_close(data->group[i].count);
			sem_unlink(data->group[i].count_name);
		}
		free(data->group);
		i = -1;
		while (++i < data->nb_philo)
		{
			sem_close(data->philo[i].meal);
			sem_unlink(data->philo[i].meal_name);
		}
		free(data->philo);
	}
	sem_close(data->write);
	sem_close(data->death);
	sem_close(data->forks);
	sem_unlink(SEM_WRITE);
	sem_unlink(SEM_DEATH);
	sem_unlink(SEM_FORKS);
}

void	action_log(t_philo *philo, int action, long long timestamp)
{
	sem_wait(philo->data->write);
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
	sem_post(philo->data->write);
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
