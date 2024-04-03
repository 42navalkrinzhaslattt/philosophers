#include "../includes/philo.h"
#include "philo.h"

//if left meal > 0 -> check in the end of the last group

void	action_log(t_philo *philo, int action, long long timestamp)
{
	pthread_mutex_lock(&philo->data->death);
	pthread_mutex_lock(&philo->data->write);
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

void	init_groups(t_data *data)
{
	int i;

	data->nb_group = 2 + (data->nb_philo % 2 == 1);
	data->group = malloc(sizeof(t_group) * data->nb_group);
	i = -1;
	while (++i < data->nb_group)
	{
		data->group[i].index = i;
		data->group[i].eat_ms = data->eat_ms;
		data->group[i].iteration = 0;
		data->group[i].round_len = data->sleep_ms;
		if ((data->nb_group - 1) * data->eat_ms > data->sleep_ms)
			data->group[i].round_len = (data->nb_group - 1) * data->eat_ms;
		data->group[i].round_len += data->eat_ms;
	}
	i = -1;
	while (++i < data->nb_philo)
		data->philo[i].group = &data->group[(i % 2 == 1) + (data->nb_group == 3) * (2 * (i != 0) * (i % 2 == 0))];
}

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

void	routine(t_philo *philo)
{
	long long	think_start;
	long long	eat_start;
	long long	sleep_start;

	eat_start = philo->group->iteration * philo->group->round_len + philo->group->index * philo->group->eat_ms;
	think_start = 0;
	sleep_start = eat_start + philo->group->eat_ms;
	action_log(philo, THINK, 0);
	while (1)
	{
		usleep((eat_start - think_start) * 1000);
		//lock everything and unlock n - 1 before
		//or while + usleep
		//pthread_mutex_lock(prev);
		pthread_mutex_lock(philo->fork_l);
		action_log(philo, FORK, eat_start);
		pthread_mutex_lock(philo->fork_r);
		action_log(philo, FORK, eat_start);
		action_log(philo, EAT, eat_start);
		usleep(philo->group->eat_ms * 1000);
		pthread_mutex_lock(&philo->meal);
			philo->left_meal--;
		pthread_mutex_unlock(&philo->meal);
		pthread_mutex_unlock(philo->fork_r);
		pthread_mutex_unlock(philo->fork_l);
		action_log(philo, SLEEP, sleep_start);
		//pthread_mutex_unlock(curr)
		usleep(philo->data->sleep_ms  * 1000);
		eat_start += philo->group->round_len;
		think_start = eat_start + philo->group->eat_ms + philo->data->sleep_ms - philo->group->round_len;
		sleep_start += philo->group->round_len;
		action_log(philo, THINK, think_start);

	}
}

int	main(int ac, char **av)
{
	t_data	data;
	int		i;

	if (ac != 5 && ac!= 6)
		exit_error(INV_ARG_STATUS, NULL);
	parse_input(&data, ac, av);
	init_groups(&data);
//	printf("number of groups: %d\n", data.nb_group);
//	printf("length of the round: %d\n", data.group[0].round_len);
//	i = -1;
//	while (++i < data.nb_philo)
//		printf("%d philosophers group is %d\n", i, data.philo[i].group->index);
	i = -1;
	while (++i < data.nb_philo)
		pthread_create(&data.philo[i].pthread, NULL, (void *)routine, &data.philo[i]);
	i = -1;
	while (++i < data.nb_philo)
		pthread_create(&data.philo[i].monitor, NULL, (void *)beholder, &data.philo[i]);
	i = -1;
	while (++i < data.nb_philo)
		pthread_join(data.philo[i].pthread, NULL);
	i = -1;
	while (++i < data.nb_philo)
		pthread_join(data.philo[i].monitor, NULL);
	free_all(&data);
	return (EXIT_SUCCESS);
}
