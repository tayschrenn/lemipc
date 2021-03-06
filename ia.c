/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ia.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aduban <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/05 16:24:40 by aduban            #+#    #+#             */
/*   Updated: 2017/05/05 17:01:49 by aduban           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lemipc.h"

int			get_score(unsigned int g, unsigned int ecount, t_lp *enemy,
		t_context *context)
{
	unsigned int	w;
	int				tmpnbally;
	t_lp			*ally;
	size_t			acount;

	w = 0;
	tmpnbally = 0;
	ally = find_all(context->map, &context->player, &isally, &acount);
	while (w < acount)
	{
		if ((enemy[g].x - 15) < ally[w].x && (enemy[g].x + 15) > ally[w].x
				&& (enemy[g].y - 15) < ally[w].y
				&& (enemy[g].y + 15) > ally[w].y)
			tmpnbally++;
		w++;
	}
	w = 0;
	while (w < ecount)
	{
		if ((enemy[g].x - 2) < enemy[w].x && (enemy[g].x + 2) > enemy[w].x &&
				(enemy[g].y - 2) < enemy[w].y && (enemy[g].y + 2) > enemy[w].y)
			tmpnbally -= 5;
		w++;
	}
	return (tmpnbally);
}

bool		get_target_coord(t_context *context, int *targetx, int *targety)
{
	unsigned int	g;
	t_lp			*enemy;
	size_t			ecount;
	int				nbally;
	int				tmpnbally;

	nbally = -100;
	tmpnbally = 0;
	g = -1;
	enemy = find_all(context->map, &context->player, &isenemy, &ecount);
	if (ecount == 0)
		return (false);
	*targetx = enemy[0].x;
	*targety = enemy[0].y;
	while (++g < ecount)
	{
		tmpnbally = get_score(g, ecount, enemy, context);
		if (tmpnbally > nbally)
		{
			nbally = tmpnbally;
			*targetx = enemy[g].x;
			*targety = enemy[g].y;
		}
	}
	return (true);
}

bool		send_attck_msg(t_context *context)
{
	int		targetx;
	int		targety;
	char	*msg;

	if (!get_target_coord(context, &targetx, &targety))
		return (false);
	asprintf(&msg, "%d;%d", targetx, targety);
	mq_send(context->mqid, context->player.id, msg, ft_strlen(msg));
	return (true);
}

void		fill_coord(char *msg_ptr, int *x_target, int *y_target)
{
	char **tab;

	tab = ft_strsplit(msg_ptr, ';');
	*x_target = ft_atoi(tab[0]);
	*y_target = ft_atoi(tab[1]);
}

void		ia(t_context *context)
{
	char	msg_ptr[1024];
	ssize_t	ret;
	int		x_target;
	int		y_target;

	if (!send_attck_msg(context))
		return ;
	ret = mq_recv(context->mqid, context->player.id, msg_ptr, 1024);
	if (ret == 0)
	{
		perror("receive msg:");
		return ;
	}
	fill_coord(msg_ptr, &x_target, &y_target);
	takeoff_player(context);
	if (abs(y_target - context->player.pos.y) >
			abs(x_target - context->player.pos.x))
		move_y(context, y_target);
	else
		move_x(context, x_target);
	place_player(context);
}
