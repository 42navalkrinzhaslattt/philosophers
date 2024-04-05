#include "../includes/philo.h"
#include "philo.h"

//if left meal > 0 -> check in the end of the last group

void	action_log(t_philo *philo, int action, long long timestamp)
{
	pthread_mutex_lock(&philo->data->death);
	pthread_mutex_lock(&philo->data->write);
	if (philo->group->index == 1)
		printf("\t");
	if (philo->group->index == 2)
		printf("\t\t");
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
		data->group[i].size = 1 * (!i && data->nb_group == 3)
			+ (data->nb_philo / 2) * (i || data->nb_group == 2);
		data->group[i].counter = 0;
		if (i == data->nb_group - 1)
			data->group[i].counter = data->group[i].size;
	}
	i = -1;
	while (++i < data->nb_philo)
		data->philo[i].group = &data->group[(i % 2 == 1)
			+ (data->nb_group == 3) * (2 * (i != 0) * (i % 2 == 0))];
}

void	beholder(t_philo *philo)
{
	int i;

//	pthread_mutex_lock(&philo->data->death);
	while (1)
	{
//		pthread_mutex_unlock(&philo->data->death);
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
			printf("beholder %d is ended\n", philo->index);
			return ;
			printf("beholder %d is not here\n", philo->index);
		}
		else if (!philo->left_meal)
			return (pthread_mutex_unlock(&philo->meal), free(NULL));
		pthread_mutex_unlock(&philo->meal);
//		pthread_mutex_lock(&philo->data->death);
	}
//	pthread_mutex_unlock(&philo->data->death);
}

void	philo_eat(t_philo *philo, int eat_start, int sleep_start)
{
	t_mutex	*forks[2];
	forks[0] = philo->fork_l;
	forks[1] = philo->fork_r;
	if (philo->index == 0)
	{
		forks[0] = philo->fork_r;
		forks[0] = philo->fork_l;
	}
	pthread_mutex_lock(forks[0]);
	action_log(philo, FORK, eat_start);
	pthread_mutex_lock(forks[1]);
	action_log(philo, FORK, eat_start);
	action_log(philo, EAT, eat_start);
	pthread_mutex_lock(&philo->meal);
	philo->left_meal--;
	philo->last_meal = eat_start;
	pthread_mutex_unlock(&philo->meal);
	usleep(philo->group->eat_ms * 1000);
	pthread_mutex_unlock(forks[1]);
	pthread_mutex_unlock(forks[0]);
	action_log(philo, SLEEP, sleep_start);
}

void	philo_round(t_philo *philo, t_data *data, int eat_start, int sleep_start)
{
//	pthread_mutex_lock(&philo->data->write);
//	printf("\t\t%d is waiting for flag\n", philo->index);
//	pthread_mutex_unlock(&philo->data->write);
	pthread_mutex_lock(&data->death);
	while (!philo->funeral)//wait for flag
	{
		pthread_mutex_unlock(&data->death);
//		printf("routine %d is here\n", philo->index);
//		printf("routine %d is not here\n", philo->index);
//		printf("index = %d flag = %d\n", philo->group->index, data->eat_flag);
		pthread_mutex_lock(&data->flag);
		pthread_mutex_lock(&data->death);
		if (philo->group->index == data->eat_flag)
			break;
		pthread_mutex_unlock(&data->flag);
	}
	pthread_mutex_unlock(&data->flag);
	pthread_mutex_unlock(&data->death);
//	pthread_mutex_lock(&philo->data->write);
//	printf("\t\t%d flag changed\n", philo->index);
//	pthread_mutex_unlock(&philo->data->write);
	philo_eat(philo, eat_start, sleep_start);
	pthread_mutex_lock(&philo->group->count);
//	printf("%d HEREEEEEEEEEEEEE\n", philo->index);
	philo->group->counter++;
	pthread_mutex_unlock(&philo->group->count);
	usleep(philo->data->sleep_ms  * 1000);
	pthread_mutex_lock(&philo->data->write);
	pthread_mutex_unlock(&philo->data->write);
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
	pthread_mutex_lock(&philo->data->death);
//	pthread_mutex_lock(&philo->data->write);
//	printf("\t%d think_start = %lld\n", philo->index, think_start);
//	printf("\t%d eat_start = %lld\n", philo->index, eat_start);
//	printf("\t%d sleep_start = %lld\n", philo->index, sleep_start);
//	pthread_mutex_unlock(&philo->data->write);
	while (philo->left_meal && !philo->funeral)
	{
		pthread_mutex_unlock(&philo->data->death);
		usleep((eat_start - think_start) * 1000);
		philo_round(philo, philo->data, eat_start, sleep_start);
		philo->group->iteration++;
		eat_start += philo->group->round_len;
		think_start = eat_start + philo->group->eat_ms + philo->data->sleep_ms - philo->group->round_len;
		sleep_start += philo->group->round_len;
//		pthread_mutex_lock(&philo->data->write);
//		printf("\t%d think_start = %lld\n", philo->index, think_start);
//		printf("\t%d eat_start = %lld\n", philo->index, eat_start);
//		printf("\t%d sleep_start = %lld\n", philo->index, sleep_start);
//		pthread_mutex_unlock(&philo->data->write);
		action_log(philo, THINK, think_start);
		pthread_mutex_lock(&philo->data->death);
	}
	pthread_mutex_unlock(&philo->data->death);
}

void	controler(t_data *data)
{
	int prev;

	pthread_mutex_lock(&data->death);
	while (!data->philo[0].funeral)
	{
//		printf("here\n");
		pthread_mutex_unlock(&data->death);
//		printf("controler is here\n");
		pthread_mutex_lock(&data->flag);
//		printf("controler is not here\n");
		prev = (data->eat_flag + data->nb_group - 1) % data->nb_group;
		pthread_mutex_lock(&data->group[data->eat_flag].count);
		if (data->group[data->eat_flag].counter == data->group[data->eat_flag].size)
		{
//			printf("before: flag = %d counter[0] = %d counter[1] = %d\n", data->eat_flag, data->group[0].counter, data->group[1].counter);
			pthread_mutex_lock(&data->group[prev].count);
			data->group[prev].counter = 0;
			pthread_mutex_unlock(&data->group[prev].count);
			data->eat_flag++;
			data->eat_flag %= data->nb_group;
//			printf("before: flag = %d counter[0] = %d counter[1] = %d\n", data->eat_flag, data->group[0].counter, data->group[1].counter);
		}
		prev++;
		prev %= data->nb_group;
		pthread_mutex_unlock(&data->group[prev].count);
		pthread_mutex_unlock(&data->flag);
		pthread_mutex_lock(&data->death);
	}
	pthread_mutex_unlock(&data->death);
}

int	main(int ac, char **av)
{
	t_data	data;
	int		i;
	t_pth	control;

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
