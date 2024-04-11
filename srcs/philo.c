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

#include "../includes/philo.h"
#include "philo.h"

//[0, eat_ms) = 2 die
//[eat_ms, round] = 1 die
//
//[0, eat_ms) = 2 die
//[eat_ms, 2 * eat_ms) = 3 die
//[2 * eat_ms, round) = 1 die
//
//        i = [0, nb_group - 1)
//[eat_ms * i, eat_ms * (i + 1)) = i + 2 die
//[eat_ms * nb_group - 1, round) = 1 die
//check before each



int	main(int ac, char **av)
{
	t_data	data;
	int		i;
	t_pth	control;

	if (ac != 5 && ac!= 6)
		return(print_error(INV_ARG_STATUS, NULL));
	if (parse_input(&data, ac, av))
		return (EXIT_FAILURE);
	init_groups(&data);
	i = -1;
	while (++i < data.nb_philo)
		pthread_create(&data.philo[i].pthread, NULL, (void *)routine, &data.philo[i]);
	pthread_create(&control, NULL, (void *)controler, &data);
	i = -1;
	while (++i < data.nb_philo)
		pthread_create(&data.philo[i].monitor, NULL, (void *)beholder, &data.philo[i]);
	i = -1;
	while (++i < data.nb_philo)
		pthread_join(data.philo[i].pthread, NULL);
	i = -1;
	while (++i < data.nb_philo)
		pthread_join(data.philo[i].monitor, NULL);
	pthread_join(control, NULL);
	free_all(&data);
	return (EXIT_SUCCESS);
}
