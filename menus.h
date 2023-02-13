! MENUS.H, Kawa's take
! --------------------
! Original by Graham Nelson
! Reformatted, mangled, and extended by Kawa
!
! Base use is as before, with the following changes:
! * Giving an Option the absent attribute hides them from the list.
!   Context-sensitivity anyone?
! * HintOption was added, to allow InvisiClues.
! * Options can have the transparent attribute set to skip over them.
!
! TODO:
! * Reconsider the prefix and nomore properties.
!
! Menu instructions "Deadline";
! 	Option -> "Credits"
! 		with description "Compiled, edited, and z-coded by:^  Digby McWiggle (digby_mcwiggle@hotmail.com)^  Steven Marsh (marsh@netally.com, www.waitingforgo.com)";
! 	Option -> "" has transparent;
! 	Option -> "InvisiClues" has transparent;
! 	Option -> "Sample Question";
! 		HintOption -> -> "Why did trusty Sgt. Duffy wear red suspenders?"
! 			with hints
! 				"He doesn't use them to gag suspects."
! 				"Have you studied his wardrobe?"
! 				[; style underline; print "To hold his pants up." style roman; ];
! 	HintOption -> "For Your Amusement"
! 		with description "(after you've finished the game)^^Have you ever tried:",
!		nomore "[No more items]", prefix " > ",
! 		hints
! 			"having the ladder analyzed after noon? (Can you guess why this happens?)"
! 			"smelling, tasting, or trying to eat the various drugs?"
! 			"swimming in the lake, taking a shower, or flushing the toilet?";

Constant KEY_ESCAPE 27;
Constant KEY_ARROWUP 129;
Constant KEY_ARROWDOWN 130;
Constant KEY_ARROWLEFT 131;
Constant KEY_ARROWRIGHT 132;

[ SetCursor y x;
#Iftrue (#version_number == 6);
	y = y * FontY;
	x = x * FontX;
#EndIf;
	@set_cursor y x;
];

Class Option
	with border 4,
	emblazon [ bar_height page pages screen_width;
		screen_width = 0->HDR_SCREENWCHARS;
		@erase_window -1;
		@split_window bar_height;
		@set_window 1;
		@set_cursor 1 1;
#Iftrue (#version_number == 6);
		@set_colour clr_fgstatus clr_bgstatus;
#IfNot;
		style reverse;
#EndIf;
		font off;
		spaces(screen_width);

#Iftrue (#version_number == 6);
		if (self.border)
			DrawBorder(self.border, false);
#EndIf;

		@set_cursor 1 1;
		style bold;
		print "  ", (name)self;
		style roman;
		if (pages ~= 1) print " [", page, "/", pages, "]";

		! _MainWindow();
#Iftrue (#version_number == 6);
		@set_colour clr_fg clr_bg;
#IfNot;
		style roman;
#EndIf;
		font off;
	],
	select [;
		self.emblazon(1, 1, 1);
		@set_window 0;
		font on;
		style roman;
		new_line;

		if (self provides description)
			return self.description();

		"[No text written for this option.]^";
	];

Class Menu class Option
	with select [ count j obj pkey  line oldline top_line bottom_line page pages options top_option screen_width screen_height lastUp;
		screen_width = 0->HDR_SCREENWCHARS;
		screen_height = 0->HDR_SCREENHLINES;
		if (screen_height == 0 or 255)
			screen_height = 18;
		screen_height = screen_height - 5;

		options = 0;
		objectloop (obj in self && obj ofclass Option)
			options++;
		if (options == 0)
			return 2;

		pages = 1 + options/screen_height;
		top_line = 2;
		page = 1;
		line = top_line;
		lastUp = false;

.ReDisplay;
		top_option = (page - 1) * screen_height;
		self.emblazon(7 + count, page, pages);

		style roman;
		font off;
		count = top_line; j = 0;
		objectloop (obj in self && obj ofclass Option && obj hasnt absent)
		{
			if (j >= top_option && j < (top_option + screen_height))
				count++;
			j++;
		}
		bottom_line = count - 1;
		oldline = 0;

		for(::)
		{
			count = top_line; j = 0;
			objectloop (obj in self && obj ofclass Option && obj hasnt absent)
			{
				if (j >= top_option && j < (top_option + screen_height))
				{
					if (obj has transparent && count == line)
					{
						if (lastUp)
							jump GoUp;
						else
							jump GoDown;
					}

					SetCursor(count + 1, 1);
					print "  ";
					if (count == line)
						style reverse;
					if (obj has transparent)
						style bold;
					print (name)obj;
					style roman;
					print "  ";
					count++;
				}
				j++;
			}

			@set_cursor -1;
			oldline = line;
			@read_char 1 -> pkey;
			if (pkey == 'N' or 'n' or KEY_ARROWDOWN)
			{
.GoDown;
				lastUp = false;
				line++;

				if (line > bottom_line)
				{
					line = top_line;
					if (pages > 1)
					{
						if (page == pages)
							page = 1;
						else
							page++;
						jump ReDisplay;
					}
				}
			}

			if (pkey == 'P' or 'p' or KEY_ARROWUP)
			{
.GoUp;
				lastUp = true;
				line--;

				if (line < top_line)
				{
					line = bottom_line;
					if (pages > 1)
					{
						if (page == 1)
						{
							page = pages;
							line = top_line + (options % screen_height) - 1;
						}
						else
						{
							page--;
							line = top_line + screen_height - 1;
						}
						jump ReDisplay;
					}
				}
				continue;
			}

			if (pkey == 'Q' or 'q' or KEY_ESCAPE or KEY_ARROWLEFT)
				break;

			if (pkey == 10 or 13 or KEY_ARROWRIGHT)
			{
				count = 0;
				objectloop (obj in self && obj ofclass Option && obj hasnt absent)
				{
					if (count == top_option + line - top_line)
						break;
					count++;
				}

				switch(obj.select())
				{
					2: jump ReDisplay;
					3: jump ExitMenu;
				}

				print "^^[Please press SPACE to continue.]^";
				@read_char 1 -> pkey;
				jump ReDisplay;
			}
		}

.ExitMenu;
		if (sender ofclass Option)
			return 2;

		font on;
		@set_cursor 1 1;
		@erase_window -1;
		@set_window 0;
#Iftrue (#version_number == 6);
		DrawBorder();
#EndIf;
		new_line;
		new_line;
		new_line;

		if (deadflag==0)
			<<Look>>;
		return 2;
	];

Class SwitchOption class Option
	with short_name [;
		print (object) self, " ";
		if (self has on)
			print "(on)";
		else
			print "(off)";
		rtrue;
	],
	select [;
		if (self has on)
			give self ~on;
		else
			give self on;
		return 2;
	];

Class HintOption class Option
	with nomore "^[No more hints.]",
	prefix [ num; print num, "> "; ],
	time_left 0,
	select [ count num pkey i;

.ReDisplay;
		self.emblazon(1, 1, 1);
		@set_window 0;
		font on;
		style roman;
		new_line;
		new_line;

		if (self provides description)
			self.description();

		num = self.#hints / 2;

		count = num;
		if (self.prefix ofclass string)
			print (string)self.prefix;
		else
			self.prefix(count);

		for (i = 0: i < self.time_left: i++)
		{
			pkey = self.&hints-->i;
			if (pkey ofclass string)
				print (string)pkey;
			else
				pkey();
			new_line;

			if (num - i > 1)
			{
				if (self.prefix ofclass string)
					print (string)self.prefix;
				else
					self.prefix(num - i - 1);
			}
			else
			{
				font on;
				self.nomore();
			}
		}

		@read_char 1 -> pkey;

		if (pkey == 10 or 13 or KEY_ARROWRIGHT)
		{
			if (self.time_left < num)
				self.time_left++;
		}
		if (pkey == 'Q' or 'q' or KEY_ESCAPE or KEY_ARROWLEFT)
			return 2;
		jump ReDisplay;
	];
