/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 07:25:59 by nam-vu            #+#    #+#             */
/*   Updated: 2024/04/11 07:25:59 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	beholder(t_philo *philo)
{
	int	i;

	while (philo->left_meal)
	{
		usleep(500);
		sem_wait(philo->meal);
		if (philo->left_meal && ft_gettime(philo->start_time + philo->last_meal)
			> philo->data->die_ms)
		{
			action_log(philo, DIE, -1);
			sem_wait(philo->data->write);
			i = -1;
			while (++i < philo->data->nb_philo + 1)
				sem_post(philo->data->death);
			i = -1;
			while (++i < 2 * philo->data->nb_philo)
				sem_post(philo->data->forks);
			return ;
		}
		sem_post(philo->meal);
	}
}

int	create_child(t_data *data, int i)
{
	data->philo[i].pid = fork();
	if (data->philo[i].pid == 0)
	{
		if (data->nb_philo > 1)
		{
			pthread_create(&data->philo[i].monitor, NULL, (void *)beholder,
				&data->philo[i]);
			pthread_detach(data->philo[i].monitor);
		}
		routine(&data->philo[i]);
		return (1);
	}
	return (0);
}

int	kill_childs(t_data *data)
{
	int	i;

	data->maniac = fork();
	{
		sem_wait(data->death);
		i = -1;
		while (++i < data->nb_philo)
			kill(data->philo[i].pid, SIGKILL);
		kill(data->control, SIGKILL);
		return (1);
	}
	return (0);
}

int	main(int ac, char **av)
{
	t_data	data;
	int		i;

	if (!parse_input(&data, ac, av))
		return (EXIT_FAILURE);
	i = -1;
	while (++i < data.nb_philo)
		if (create_child(&data, i))
			return (EXIT_SUCCESS);
	if (controler(&data))
		return (free_all(&data), EXIT_SUCCESS);
	if (kill_childs(&data))
		return (free_all(&data), EXIT_SUCCESS);
	i = -1;
	while (++i < data.nb_philo)
		waitpid(data.philo[i].pid, NULL, 0);
	waitpid(data.control, NULL, 0);
	return (EXIT_SUCCESS);
}
