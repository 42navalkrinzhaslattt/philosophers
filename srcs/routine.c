/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 07:25:59 by nam-vu            #+#    #+#             */
/*   Updated: 2024/04/11 07:25:59 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"
#include "philo.h"

void	beholder(t_philo *philo)
{
	int i;

	while (1)
	{
		pthread_mutex_lock(&philo->meal);
		if (philo->left_meal && ft_gettime(philo->start_time + philo->last_meal) > philo->data->die_ms)
		{
			action_log(philo, DIE, -1);
			pthread_mutex_lock(&philo->data->death);
			i = -1;
			while (++i < philo->data->nb_philo)
				philo->data->philo[i].funeral = 1;
			pthread_mutex_unlock(&philo->data->death);
			pthread_mutex_unlock(&philo->meal);
			return ;
		}
		else if (!philo->left_meal)
			return (pthread_mutex_unlock(&philo->meal), free(NULL));
		pthread_mutex_unlock(&philo->meal);
	}
}

void	philo_eat(t_philo *philo, int eat_start)
{
	t_mutex	*forks[2];

	forks[0] = philo->fork_l;
	forks[1] = philo->fork_r;
	if (philo->index % 2 == 0)
	{
		forks[0] = philo->fork_r;
		forks[1] = philo->fork_l;
	}
	pthread_mutex_lock(forks[0]);
	action_log(philo, FORK, eat_start);
	pthread_mutex_lock(forks[1]);
	action_log(philo, FORK, eat_start);
	action_log(philo, EAT, eat_start);
	pthread_mutex_lock(&philo->meal);
	if (philo->left_meal > 0)
		philo->left_meal--;
	philo->last_meal = eat_start;
	pthread_mutex_unlock(&philo->meal);
	usleep(philo->group->eat_ms * 1000);
	pthread_mutex_unlock(forks[1]);
	pthread_mutex_unlock(forks[0]);
}

void	philo_round(t_philo *philo, t_data *data, int eat_start, int sleep_start)
{
	pthread_mutex_lock(&data->death);
	while (!philo->funeral)
	{
		pthread_mutex_unlock(&data->death);
		pthread_mutex_lock(&data->flag);
		if (philo->group->index == data->eat_flag)
		{
			pthread_mutex_unlock(&data->flag);
			pthread_mutex_lock(&data->death);
			break ;
		}
		pthread_mutex_unlock(&data->flag);
		pthread_mutex_lock(&data->death);
	}
	pthread_mutex_unlock(&data->death);
	philo_eat(philo, eat_start);
	pthread_mutex_lock(&philo->group->count);
	philo->group->counter++;
	pthread_mutex_unlock(&philo->group->count);
	if (!philo->left_meal)
		return ;
	action_log(philo, SLEEP, sleep_start);
	usleep(philo->data->sleep_ms * 1000);
}

void	routine(t_philo *philo)
{
	long long	think_start;
	long long	eat_start;
	long long	sleep_start;

	eat_start = philo->group->index * philo->group->eat_ms;
	think_start = 0;
	sleep_start = eat_start + philo->group->eat_ms;
	action_log(philo, THINK, 0);
	if (philo->data->nb_philo == 1)
		return ;
	pthread_mutex_lock(&philo->data->death);
	while (philo->left_meal && !philo->funeral)
	{
		pthread_mutex_unlock(&philo->data->death);
		usleep((eat_start - think_start) * 1000);
		philo_round(philo, philo->data, eat_start, sleep_start);
		eat_start += philo->group->round_len;
		think_start = eat_start + philo->group->eat_ms + philo->data->sleep_ms - philo->group->round_len;
		sleep_start += philo->group->round_len;
		action_log(philo, THINK, think_start);
		pthread_mutex_lock(&philo->data->death);
	}
	pthread_mutex_unlock(&philo->data->death);
}

void	controler(t_data *data)
{
	int	prev;
	int	first;
	int	second;

	pthread_mutex_lock(&data->philo[data->nb_philo - 1].meal);
	pthread_mutex_lock(&data->death);
	while (!data->philo[0].funeral && data->philo[data->nb_philo - 1].left_meal)
	{
		pthread_mutex_unlock(&data->death);
		pthread_mutex_unlock(&data->philo[data->nb_philo - 1].meal);
		pthread_mutex_lock(&data->flag);
		second = data->eat_flag;
		prev = (data->eat_flag + data->nb_group - 1) % data->nb_group;
		first = prev;
		if (first > second)
		{
			first = data->eat_flag;
			second = prev;
		}
		pthread_mutex_lock(&data->group[first].count);
		pthread_mutex_lock(&data->group[second].count);
		if (data->group[data->eat_flag].counter == data->group[data->eat_flag].size)
		{
			data->group[prev].counter = 0;
			data->eat_flag++;
			data->eat_flag %= data->nb_group;
		}
		pthread_mutex_unlock(&data->group[second].count);
		pthread_mutex_unlock(&data->group[first].count);
		pthread_mutex_unlock(&data->flag);
		pthread_mutex_lock(&data->philo[data->nb_philo - 1].meal);
		pthread_mutex_lock(&data->death);
	}
	pthread_mutex_unlock(&data->death);
	pthread_mutex_unlock(&data->philo[data->nb_philo - 1].meal);
}