! You know that thing where Shogun gives you a proper
! menu instead of having you type, when you start the
! game or die?
! Normally I would reverse engineer how it did that, but
! instead this is my own original work, with only a bit
! of copying from menus.h.
! Note that this is V6 only, since we @set_cursor in the
! text window.

! Personally, I went and replaced part of PunyInform's
! main routine, the part where it asks what you want to
! do now that deadflag has been set, with this:
! 	#Iftrue (#version_number == 6);
! 		verb_word = DoInlineMenu(restartMenu);
! #IfNot;
!		_ReadPlayerInput(true);
! 		verb_word = parse-->1;
! #EndIf;
! where the IfNot part is what was originally there.
Array restartMenu table
!	Text to display			Value to return
	" Restore a game "		'restore'
	" Restart this game "	'restart'
	" Quit "				'quit';

! Replace YesOrNo OriginalYesOrNo;
! [ YesOrNo i y k;
! #Iftrue (#version_number == 6);
! 	return DoInlineMenu(yesNoMenu, false);
! #IfNot;
! 	return OriginalYesOrNo();
! #EndIf;
! ];
! Snap-in replacement right there.
Array yesNoMenu table
	" Yes "	true
	" No  "	false;

! Already using my menus.h? Then leave these out.
Constant KEY_ARROWUP 129;
Constant KEY_ARROWDOWN 130;
Constant KEY_ARROWRIGHT 132;


! And now, the big code itself.

[ DoInlineMenu options clear i y k;
	@check_arg_count 2 ?CarryOn;
	clear = true;
.CarryOn;

	for (k = 0: k < options-->0: k++)
		new_line;

! First, get our font height...
	@get_wind_prop 0 13 -> k;
	k = (i & $FF00) / 255;
! Then, get our current position...
	@get_wind_prop 0 4 -> y;
! Finally, go back up N lines.
	y = y - (k * (options-->0 - 1));

	@set_cursor -1;
	i = 0;

.ReDisplay;
	@set_cursor y 1;

	for (k = 0: k < options-->0 / 2: k++)
	{
		if (i == k) style reverse;
		print (string)options-->(1 + (k * 2));
		style roman;
		new_line;
	}

	@read_char 1 -> k;
	if (k == 'N' or 'n' or KEY_ARROWDOWN)
	{
		i++;
		if (i == options-->0 / 2)
			i = 0;
		jump ReDisplay;
	}
	if (k == 'P' or 'p' or KEY_ARROWUP)
	{
		if (i == 0)
			i = options-->0 / 2;
		i--;
		jump ReDisplay;
	}
	if (k == 10 or 13 or KEY_ARROWRIGHT)
	{
		if (clear)
		{
			@set_cursor y 1;
			for (k = 0: k < options-->0: k++)
			{
				FastSpaces(HDR_SCREENWCHARS->0);
				new_line;
			}
			@set_cursor y 1;
		}

		return options-->(2 + (i * 2));
	}
];
