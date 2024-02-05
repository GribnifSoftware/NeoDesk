\screen Gadgets
\screen Window colors and keyboard equivs.
Window colors and keyboard equivs.
----------------------------------

  This dialog gives you control over the colors and fill patterns of the
  gadgets in NeoDesk's windows. It also allows you to change the keyboard
  keys that can be used to achieve common window operations.

  If you use Atari's XCONTROL panel and have a CPX module to set the window
  colors, then NeoDesk can read these settings. Choose the "Use system
  window colors" button if you would prefer to control the colors of
  NeoDesk's windows with the CPX. In this mode, only the 3D attribute
  of the gadgets can be changed.

  In order to be compatible with the method used by Atari to control window
  colors, when the "Use system window colors" option is selected,
  some of the window gadgets NeoDesk uses inherit the same colors as
  other gadgets. For instance, if you change the color of the Full
  box using WINDOWS.CPX, the color of the Cycle gadget will also change.
  When "Use system window colors" is not selected, all gadgets can be
  set independently.

  By clicking on the "Topped" button, you can toggle the display between
  editing the attributes of topped windows and non-topped windows.

  To copy the attributes of one gadget to another, click and hold the left
  mouse button on the currently selected gadget. An outline box will
  appear, which can be dragged to the destination gadget.

  The keyboard equivalent for the currently selected window gadget can also
  be changed. If one is available, you will see the current setting in the
  bottom of this dialog. Note that selecting the area above the vertical
  scroll drag box ("page up") allows you to change a different keyboard
  equivalent than selecting the area below that gadget ("page down").
  
  The "Read" button will allow you to press the keyboard key to assign to
  the currently selected window function. The "Clear" button will erase the
  current key. The dialog will show the string "???" to indicate that the
  key is undefined.

  You should remember to \#Save Configuration\# after making changes to
  this dialog so that your changes become permanent.

Related topics:  \#Window preferences\#
\end

\screen Button Colors
\screen Dialog colors
Dialog Colors
-------------

  This dialog allows you to set the way in which various components of
  other dialogs are displayed.

  NeoDesk allows you to have separate appearances for several types of
  objects contained within dialogs. The "Type" box contains a list of the
  four different types of objects that can be assigned these special
  attributes. The "Sample" shows you one object of each of these types, so
  that you can gauge how they will look in actual dialogs.

  The "Attributes" section is where you can specify how the currently
  selected object should look:

    Border
    \x3 AES 4 3D: a three-dimensional effect, compatible with Atari's AES
      version 4. In this mode, the text color, fill color, and fill pattern
      are modified by BCOLORS.CPX, not the Task Manager. For the Box
      object, you will probably also want to select the Outline option. In
      this mode, Any text in the object will "move" when it is selected.
    \x3 3D: the original Geneva-style three-dimensional effect. The text
      of the object will "move" when it is selected.
    \x3 Outline: produces a line around the outside of the object.
    \x3 Shadow: a shadow on the lower-right edge of the object.
    \x3 Round: the corners of the outer edge are rounded, instead of
      square.  Note that if Outline is also used, the thick part of the
      object's border is moved to the outside.
    \x3 Color: the left and right arrows will cycle through the available
      colors, and clicking on the color sample will produce a popup menu of
      colors.

    Text
    \x3 Shadowed: the text has a shadow behind it. The shadow's color is the
      text color chosen below. The text itself is always drawn in either
      black or white.
    \x3 Bold: bold text. This can even be combined with the Shadowed effect.
    \x3 Opaque: affects how text is drawn. When this flag is set, the area
      around the actual dots that make up a character of text are left
      white. Otherwise, the fill pattern of the object will show through.

    Fill
    \x3 Pattern: this is a pop-up menu which lets you choose from any of
      the eight pre-defined fill patterns. You can also move from one to
      the next by clicking on the left or right arrow buttons.
    \x3 Color: this is a pop-up menu with all of the colors available in the
      resolution you are currently using. (Some colors will be black if the
      current screen resolution does not support at least 16 colors at
      once.) You can use this option to define what color is used when
      drawing the fill pattern of the currently selected object.

  You should remember to \#Save Configuration\# after making changes to
  this dialog so that your changes become permanent.

Related topics: \#Dialog preferences\#
\end

\screen 1 Column
\screen >= 1 Column
1 Column / >= 1 Column
----------------------

  This entry (it is a toggle) will let you choose the format used to
  display items as text. When 1 Column is selected, all the items are
  displayed in a single column. If >= 1 Column is selected, the items are
  displayed in multiple columns (as long as there is enough horizontal
  space within the window).

Related topics:  \#View Menu\#
\end

\screen Accessories...
\screen NeoDesk desk accessories
NeoDesk Desk Accessories
------------------------

  The NeoDesk desk accessories dialog is where you can specify the filename
  (without the ".ACC" ending) of any NeoDesk Accessories you have loaded.

  If you are using the NeoDesk CLI, you can also specify which accessory
  this is (so that it can be used as the NeoDesk \#Batch File Interpreter\#).

  You can also \#open\# any of these special desk accessories directly from the
  keyboard by holding down the [CONTROL] key and selecting the appropriate
  number (as shown under the Key column).

Related topics:  \#Settings Menu\#
\end

\screen Active Icons
Active Icons
------------

  NeoDesk also has the ability to use Active Icons. With this feature, you
  can select any data file (or group of files) and drag them over to any
  program icon and release them over that icon. NeoDesk will automatically
  run that program and give it those files as parameters.

  A folder can also be passed to a program by dragging that folder over to
  the program icon and releasing it. NeoDesk will again run the program and
  give it the path and name of the folder.

  For example, you can pick up a document, drag it over to your word
  processor, and release it over it. The word processor will then run and,
  if it supports this feature, load the document automatically. See the
  Passing Files as Parameters section for more information.

Related topics:  \#Open\#
\end

\screen Name Conflicts
\screen Name conflict during rename:
\screen Name conflict during copy:
\screen Name conflict during move:
\screen An item with this name already exists!
File & Folder Name Conflicts
----------------------------

  When the Confirm overwrites button is selected, if you try \#copying\# or
  moving an item to where there already is an item with the same name, a
  filename conflict dialog will appear. This will also happen if you try to
  rename an item using the name of an item that already exists.

  At this point you can tell NeoDesk to overwrite the file (Ok!), Skip it,
  or Cancel the entire operation.

  You can also rename the new file (the one being copied or moved), so that
  it will not conflict with the original file that was there before. In
  addition, you can also change the name of the original file (in other
  words, you can rename either one of the files).

  The file size, time, and date of each file will be displayed, to assist
  you in making your decision.

Related topics:  \#Using NeoDesk\#  \#Copying/Moving Items\#  \#Renaming Items\#
\end

\screen Background Windows
Background Windows
------------------

  All NeoDesk windows can be fully manipulated from the background just
  like a topmost window. This is done by holding down the right mouse
  button while still using the left mouse button as usual. Any background
  NeoDesk window can be dragged, resized, scrolled, closed, and manipulated
  just as if it were the topmost window. The same method can be used to
  select and \#open\# any icons within those background windows.

  Unless you are running Geneva (or another similar environment), this
  feature only works when a NeoDesk window is the topmost window.

Related topics:  \#Windows\#
\end

\screen Batch Files
\hidden Batch file
Batch Files
-----------

  While batch files are not programs, they are treated in a similar manner.
  If you \#open\# a batch file with a ".BAT" file ending, NeoDesk will execute
  the program that you have entered in the \#Paths...\# dialog in the Settings
  desktop drop-down menu, as your batch file interpreter, and give it the
  batch file you tried opening as a parameter. If you open a batch file
  ending in a ".BTP" file ending, NeoDesk will first present you with a
  \#Parameters for execution\# dialog before running the batch file
  interpreter. Here, in this dialog, you can see the name and path of the
  batch file you tried opening and you can add any additional parameters
  you wish to send to the batch file interpreter.

  If you are using the NeoDesk CLI as your batch file interpreter (also
  known as a command line interpreter or CLI) in the \#Accessories...\# dialog
  under the Settings desktop drop-down menu, this will override any entered
  batch file interpreter.

Related topics:  \#Open\#  \#Batch File Interpreter\#
\end

\screen Batch File Interpreter
\screen Command Line Interpreters
\screen CLI's
Batch File Interpreters
-----------------------

  Batch files are text files that contain a list of commands belonging to a
  Batch File Interpreter (also known as a Command Line Interpreter or CLI),
  that can automate a series of operations.

  For batch files to work correctly, you need to enter the name and path of
  your batch file interpreter in the \#Paths...\# dialog in the Settings
  desktop drop-down menu. Once its name has been entered here, pressing the
  [CONTROL]-[B] key combination will execute the interpreter.

  If you are using the NeoDesk CLI, you do not need to do this.  Instead,
  you need to enter its filename in the \#Accessories...\#  dialog and select
  the CLI button. See the NeoDesk CLI manual for more information.
  Whenever you \#open\# a batch file (usually a file ending in either ".BAT" or
  ".BTP"), NeoDesk will invoke the installed interpreter and give it the
  name and path of the batch file you tried opening. This is usually done
  in the form of a parameter.

  It is then up to the interpreter to open the batch file and read its
  contents (hopefully resulting in the execution of its commands). By
  adding the interpreter's "exit" command to the end of your batch files,
  you will return to the NeoDesk desktop as soon as the batch file has
  finished running.

  NeoDesk supports two types of batch files, which share a special batch
  file icon.

  \x3 BAT - Regular batch files. Double-clicking on them executes them
    instantly (via the installed interpreter).

  \x3 BTP - Batch Takes Parameters batch file. These are batch files that
    require additional parameters before running. When you double-click on
    one of these files, the \#Parameters for Execution\# dialog appears.  This
    lets you enter any parameters that this batch file requires.

  When NeoDesk first loads into memory, it automatically searches for and
  executes a batch file called NEOAUTO.BAT (if it can find it). This allows
  you to have a batch file run every time NeoDesk is loaded.

  Not all command line interpreters can be used as a batch file
  interpreter. If the command line interpreter ignores any batch file
  passed on to it as a parameter, then it will not work. Also, some command
  line interpreters will only read batch files with the BAT file ending,
  ignoring those with the BTP file ending.

Related topics:  \#Open\#  \#Batch Files\#
\end

\screen Clipboard Icon
Clipboard Icon
--------------

  The clipboard icon represents the NeoDesk File Clipboard. It is a
  temporary storage place where you can place any files or \#folders\# while
  you switch disks, change directories, or search through your hard disk.
  In a way, it is similar to an automatically expanding and shrinking
  ramdisk.

  From NeoDesk, it is used like any other drive. Copy or move files and
  folders to it that you wish to hold in memory. Its maximum size is
  equivalent to the total amount of free memory available in your computer
  (or as set in the \#Memory...\# dialog in the Settings desktop drop-down
  menu). To \#erase\# all the files in the clipboard, drag it to the trashcan.

  The File Clipboard is NOT a ramdisk. It does not have a drive letter. It
  is also automatically cleared every time you run a program (in
  non-multitasking environments) or load a desktop configuration file. If
  there is something in the File Clipboard, you will be warned before it is
  erased. You also cannot run a program from it.  Of course, you can still
  run a ramdisk in conjunction with the File Clipboard.

Related topics:  \#Icons\#
\end

\screen Close Folder
Close Folder
------------

  This entry is available if the window is displaying a path that is at
  least one folder deep. When selected it will close that folder and the
  window will then display the previous directory (the one that the folder
  is actually stored in).

Related topics:  \#File Window Menu\#  \#Window Close Button\#  \#Close Window\#
\end

\screen Close Window
Close Window
------------

  This entry closes the selected window, regardless of what path it is
  displaying. This same effect can be achieved by hoding the
  [CONTROL] key while clicking on a window's Close box.

Related topics:  \#File Window Menu\#  \#Window Close Button\#  \#Close Folder\#
\end

\screen Creating Folders
Creating & Using Folders
------------------------

  Folders are used to store any number of items (including other folders)
  within them. In NeoDesk, they basically work just as they did in the
  built in desktop.

  You can create a \#new folder\# in any window by selecting the Create
  Folder... command from the File window drop-down menu. You can store
  folders within folders, up to seven deep. This makes it very easy to
  organize your hard disk or floppies with folders.

  With NeoDesk, you can place any folder on the desktop itself. You can
  then \#open\# a window to that desktop folder by opening it. It can also be
  used for desktop operations (copy and move).

Related topics:  \#Using NeoDesk\#
\end

\screen Creating Groups
\screen Group
\hidden Groups
Creating & Using Groups
-----------------------

  A "Group" is a special file that contains a visual list of the names and
  paths of items that are stored somewhere else on your system. Inside
  NeoDesk, a group opens into a window which shows the icons for the items
  within it. These icons are place holders for the original files and
  programs.

  To create a group, select the \#New Group...\# command from the File window
  drop-down menu. You will be prompted to enter the group's name along with
  other related information.  Here you can enter the actual filename used
  to store the group on the disk, the name it should show in a NeoDesk
  window, and a description (which will be used whenever you Show
  Information... on that group).

  Once created, the group will open into a window. You can drag to it icons
  representing files, \#folders\#, and other groups. Unlike a regular file
  window, group windows have several key differences:

  \x3 The icons placed within it can be placed in any window position, and
    are therefore not snapped automatically to an invisible grid (though
    you can clean them up with the Snap to Grid... feature).

  \x3 You can change the name of any item inside a group to be up to 20
    characters long, with upper and lowercase characters and spaces.  This
    only affects the icon inside the group and does not affect the original
    filename.

  \x3 An item in an open group can be started with a single keypress.  See
    \#Change Item...\# for more information.

  \x3 Any settings you change in a group (including window size and location)
    will be preserved, as long as you save the group settings.  This is
    done under the File window drop-down menu of a group window.

  You can delete a group by dragging its icon to a trashcan. The original
  files represented within it will not be deleted.

Related topics:  \#Using NeoDesk\#   \#Icons\#
                 \#File Icons\#      \#Opening Groups\#
                 \#New Group...\#    \#Change Item...\#
                 \#Show Info on Groups\#
\end

\screen Custom Icons
Custom Icons
------------

  NeoDesk expands upon the basic \#file icons\# by allowing you to define
  custom icons to represent any folder, group, file, or program. NeoDesk
  comes with a large array of custom icons. You can create new ones, or
  edit the current ones by using the Icon Editor. Refer to the
  NeoDesk manual for more information.

Related topics:  \#Icons\#  \#File Icons\#  \#Assigning New Drive Icons\#
\end

\screen INF Files...
\screen Default INF/MAC/NOT names
Default INF/MAC/NOT Names
-------------------------

  The Default INF/MAC/NOT names dialog is where you can specify the
  filenames used by NeoDesk when saving its desktop preferences, which can
  include three kinds of files - ".INF" (desktop information), ".MAC"
  (macros), and ".NOT" (desktop notes). Here is also specified which
  information file is used for which screen resolution.

  New screen resolutions are added to the list whenever NeoDesk first runs
  in a new resolution. Up to ten resolutions can be kept in the list.

  One common use of this dialog is to tell NeoDesk to use the same
  information files in more than one resolution. For example, if you want,
  you can have medium and high resolution use the same preferences by
  making their filenames identical.

  You can also specify the name of a default preferences file to be used
  when loading into a resolution that does not have a pre-configured
  preference file. This can either be the name of one of your current
  preference files, or a completely separate one.

Related topics:  \#Settings Menu\#
\end

\screen Delete
Delete
------

  Selecting the Delete entry while one or more desktop \#drive icons\# are
  selected will \#Erase\# their contents. This is identical to dragging a
  disk icon to the built-in trashcan. This should NOT be confused with the
  recoverable trashcan, which has its own desktop icon, and cannot delete
  drives.

  If one or more \#file icons\# on the desktop are selected when this option is
  chosen, they will simply be removed from the desktop. The original files
  will not be affected.

  This entry will also delete any selected items in a window. This command
  is identical to dragging the items to the desktop's built-in trashcan.

Related topics:  \#File Desktop Menu\#
\end

\screen Deleting Items
\screen Delete items
\hidden Delete item
Deleting Files & Folders
------------------------

  Files and folders can be \#delete\#d by dragging them to the \#trashcan icon\#.
  As long as the "Confirm deletes" option in the \#File...\# dialog in the
  Settings desktop drop-down menu is enabled (the default setting), you
  will be asked for confirmation before the files are deleted.

  This dialog displays in the upper left, the path from which the items
  will be deleted.

  With the "Count items" button selected, NeoDesk will count how many files
  and folders are waiting to be deleted. However, counting a large number
  of files can take additional time.

  With the "In background" button selected, NeoDesk will delete the items
  in the background. This allows you to continue working on your machine
  while the items are being deleted. If you are using a multitasking
  environment, you can even go on and continue using another program. On a
  singletasking system you are limited to the desktop and desk accessories.

  NeoDesk file and folder operations operate through a queue. If you
  attempt to copy, move, or delete additional items while a similar
  operation is already taking place, your request will be placed in the
  queue. As soon as the current operation is completed, the appropriate
  dialog will appear ready for the next operation.  However, canceling any
  active operation will also cancel any operations waiting in queue.

  The "Filter" button specifies if you want the operation to be \#filter\#ed
  through the Filtering dialog. The settings of that dialog are set using
  the Edit... button. Selecting this button opens us the filtering dialog,
  where you can set the type of filtering that you want applied to this
  operation.

  When the Filter button is selected, no folders will be deleted.  This is
  intentional, to allow for filter combinations that might otherwise delete
  folders that you do not want deleted.

  The dialog will remain visible (updating its information as the operation
  continues) until it is completed. While the operation is taking place,
  the "Ok!" button will change to read "Pause". This will let you
  temporarily pause the operation. The In background button can still be
  selected while the operation is taking place.

  Taking any action which forces this dialog to close will cause the
  operation to terminate. This includes closing its window, closing the
  NeoDesk desktop window (if running as a desk accessory), or running a
  program in a non-multitasking system.

Related topics:  \#Using NeoDesk\#  \#Filtering Items\#
\end

\screen Desktop
Desktop
-------

  The desktop is the area under the desktop drop-down menu where all the
  \#icons\# and \#windows\# are displayed. This area is usually filled with a
  pattern or solid color, but it can also be replaced with a picture.

  The pattern used to draw the desktop in NeoDesk can be modified using the
  NeoDesk Icon Editor. A number of options are allowed, which include the
  ability to import a pattern, edit it, and colorize it. Refer to the
  NeoDesk manual for more information.

Related topics:  \#Using NeoDesk\#     \#Desktop...\#
                 \#Desktop Notes...\#  \#Using Desktop Notes\#
                 \#Desktop Pictures\#
\end

\screen Desktop Drop-Down Menu
Desktop Drop-Down Menu
----------------------

  The menu bar at the top of the screen is the desktop drop-down menu. It
  contains most of the desktop oriented NeoDesk commands. All of these
  commands have a keyboard equivalent, listed to the right of the command,
  which can be used to call that command.

  The keyboard equivalents require that you either hold down the [CONTROL]
  or [ALTERNATE] keys while pressing an additional key, or that you press
  the displayed key. To save space in the drop-down menus, the symbol "^"
  represents the [CONTROL] key and the window full symbol () represents
  the [ALTERNATE] key. For example, if a menu has "^O" as its keyboard
  equivalent, that means that you should hold down the [CONTROL] key while
  pressing the "O" key. An up arrow () means that you should hold down
  either [SHIFT] key while selecting any other keys shown. [F1] and [ESC]
  mean that you should press the keys with those labels.

  If a menu entry is currently not available, it will be displayed in light
  gray text. Available menu entries are displayed in solid black text and
  will reverse (what is white becomes black and what is black becomes
  white) when you move the mouse over them.

  Note:  Due to size constraints, the low resolution drop-down menus do not
         have enough space to list their keyboard equivalents, though they
         are all still there. Many of the menu entries have also been
         abbreviated, but they are all in the same locations.

Menus:  \#NeoDesk Menu\#   \#File Desktop Menu\#
        \#Settings Menu\#  \#Options Desktop Menu\#

Related topics:  \#Using NeoDesk\#  \#Desktop\#
\end

\screen Desktop Icons
Desktop Icons
-------------

  NeoDesk lets you place any file, folder, or group icon directly on the
  desktop. This desktop icon then represents the original item and gives
  you an easier and faster way to access that item.

  This is done by dragging any icon from an open window onto the desktop.
  When the icon is released over the desktop, a copy of that item is left
  on the desktop where you placed it. The original file, folder, or group
  is not affected in any way.

  This desktop icon tells NeoDesk where it can find the original item.
  Whenever you open a desktop icon, NeoDesk will know where it is located
  and open it for you. Trying to open a desktop file icon whose original
  file is no longer stored in the original path and filename, or after
  having removed a disk containing that file, will result in an appropriate
  error message. When you open a folder desktop icon, a window is opened to
  that folder's path. If the folder is no longer present, the window will
  back up to the next valid path.

  You can open a window to a desktop icon's path by holding down the
  [CONTROL] key while double-clicking on it. This technique will also work
  for icons which are inside a group window. If you want to see what
  filename and path is currently associated with any desktop icon, hold
  down the [CONTROL] key and press and hold the left mouse button on that
  icon. The desktop menu bar will display its path and filename.

  Desktop file, folder, group, and drive icons can be copied or moved. You
  can also copy or move items from a window to a desktop folder, desktop
  group, or desktop drive icon. This action is identical to \#copying\# or
  moving those items into a window open to that folder or group.

  Desktop icons can have their icon name changed by selecting the desktop
  icon and then selecting the \#Install Desktop Icon...\# entry from the
  Options desktop drop-down menu.  Any desktop icon can also be given a
  unique keyboard equivalent using the Install Desktop Icon... command.
  Once a desktop icon has a unique letter or number assigned to it, it can
  be selected or opened by pressing that character. Pressing the icon's
  character once will select it, pressing it a second time will open it.
  This method is similar to the method used to open drive icons (which use
  uppercase letters).

Related topics:  \#Using NeoDesk\#  \#Icons\#  \#File Icons\#
\end

\screen Using Desktop Notes
\hidden Desktop Notes
Using Desktop Notes
-------------------

  NeoDesk lets you easily write Desktop Notes directly on the desktop
  itself. This can be very useful for jotting down quick notes, phone
  numbers, reminders, and other information.

  Desktop Notes are activated by double-clicking on the desktop itself. A
  cursor appear and you can begin typing. Each note can be of any length,
  and there's almost no limit to how many you can have. Pressing the
  [RETURN] key will end the note and return the mouse pointer.

  Editing a note is done the same way, by double-clicking on the existing
  note. [RIGHT ARROW] and [LEFT ARROW] can be used for moving the cursor
  within the text. [SHIFT]-[LEFT ARROW] and [SHIFT]-[RIGHT ARROW] can be
  used to jump to the beginning and end of each line. The [BACKSPACE] key
  will erase any character to the left of the cursor, while the DELETE key
  will erase any character to the right of the cursor. Pressing the [ESC]
  key will clear the line.

  You can select any SpeedoGDOS or GDOS font for your Desktop Notes. This
  is done by selecting the Desktop Notes button inside the Desktop... entry
  in the Settings desktop drop-down menu.

  Here you can select Font Name, Point Size, and Color of the font that you
  want to use for your Desktop Notes. You can also select if the Desktop
  Notes are opaque. The upper left portion of the dialog displays a sample
  of what the resulting Desktop Notes will look like.

  Desktop Notes are placed on the desktop using a grid system based on the
  size of the font being used. Therefore, any changes to the font and size
  of the text will result in a different desktop placement. Be careful when
  enlarging a font, or you might find that your desktop notes have
  literally been placed beyond the screen limits!

  Desktop Notes are stored in their own (.NOT) files. This is done with the
  \#Save Configuration\#... entry under the Options desktop drop-down menu.
  The default note file usually has the same filename as the current
  desktop, but can be given any other name. This allows you to save and
  load an unlimited number of different note files per desktop.

  To load a different desktop note file (.NOT), you need to first select
  the file you want to load and then select the \#Load Configuration\# entry
  from the Options desktop drop-down menu.

Related topics:  \#Using NeoDesk\#  \#Desktop...\#  \#Desktop Notes...\#
\end

\screen Desktop Notes¿
\screen Desktop Notes...
Desktop Notes
-------------

  The "Desktop Notes..." button in the "Desktop..." dialog lets you choose
  the style of the desktop notes that are written on the desktop.

  Here you can select the "Font name", "Point size", and "Color" of the
  text that you want to use for your Desktop Notes. You can also select if
  the Desktop Notes are opaque. The upper left portion of the dialog
  displays a sample of what the resulting Desktop Notes will look like.

  Selecting the "Color" button will display a list of the 16 standard
  system colors. If you current resolution cannot display all 16 colors,
  the remaining colors will be shown in black. The arrows at either side of
  the button let you scroll through the different colors available.

  The "Name" button indicates which font is currently selected for desktop
  note use. Clicking on the font button will display the names of the
  available fonts (including GDOS and SpeedoGDOS fonts).

  The "Point size:" selection allows you to choose between the different
  sizes available for the currently selected font.

  Desktop notes are placed on the desktop using a grid system based on the
  size of the font being used. Therefore, any changes to the font and size
  of the text will result in a different desktop placement. Be careful when
  enlarging a font, or you might find that your desktop notes have
  literally been placed beyond the screen limits!

Related topics:  \#Settings Menu\#  \#Desktop...\#  \#Using Desktop Notes\#
\end

\screen Desktop Pictures
Desktop Pictures
----------------

  You can use a picture on the desktop to replace the desktop pattern and
  it will always appear under your \#windows\# and \#icons\# when at the NeoDesk
  desktop. There are two ways to display a picture, either by centering it
  on the screen or by tiling it across the screen (creating an effect
  similar to that of wallpaper). The desktop picture is set through the
  \#Desktop...\# entry in the Settings desktop drop-down menu.

Related topics:  \#Using NeoDesk\#  \#Desktop\#
\end

\screen Desktop...
\screen Desktop preferences
Desktop Preferences
-------------------

  Here you can specify most of the settings that affect the desktop
  background and the desktop notes.

  The "Display in a window" option, when selected, forces the entire
  NeoDesk desktop to be displayed inside a window. The NeoDesk desktop menu
  bar will also be included inside this window. This is identical to how
  NeoDesk operates when run as a desk accessory. When running under Geneva
  or MultiTOS, the desktop window will remain at the bottom of other
  windows. It can only be topped (brought to front) by selecting its Window
  Name (drag bar) area. You can, however, use any part of the desktop
  without topping it.

  The "Background picture" options control how and what picture NeoDesk
  displays on its desktop background. The "Select..." button lets you
  select a bitmap picture in a number of popular formats (IMG, NEO, PI1-3,
  PC1-3, TN1-3, TNY, BMP). If the picture you load does not match the
  current number of screen colors, it will be dithered to monochrome. The
  "Use picture" button indicates if the picture should be displayed, and
  the "Centered" and "Tiled" buttons indicate how it should be shown.
  Centered pictures from other screen resolutions can have their appearance
  corrected with the "Options" button, which calls up the \#Picture Options\#
  dialog.

  If your system is low on memory, you might want to keep in mind that
  desktop pictures consume additional memory (the exact amount depending on
  the size of your screen).

  The "Use Picture Colors" settings let you specify how the picture colors
  should be used. If "All" the colors are used, there's a chance that the
  system colors (the first two, which are used for drawing text, windows,
  and menus) could be changed. This will, however, give you the most
  accurate picture colors. The "All but system" option does not allow the
  picture to change the system colors. Finally, the "None" option will
  force NeoDesk to ignore the picture's colors and use the current system
  colors.

  Sometimes, when loading a desktop picture, you will notice that your
  screen colors have been adversely affected. This is because the picture
  has changed the system color palette. Holding down the [CONTROL]-[LEFT
  SHIFT]-[ALTERNATE] keys while pressing the "*" on the numeric keypad will
  restore the default color palette.

  The "Desktop Notes..." button lets you choose the style of the desktop
  notes that are written on the desktop.

Related topics:  \#Settings Menu\#  \#Desktop Pictures\#
\end

\screen Picture Options
\screen Picture viewer options
\screen Centered desktop picture
Picture Options
---------------

  Both the \#Miscellaneous...\# settings and \#Desktop preferences\#
  dialogs use this same sub-dialog. For the picture viewer, this dialog
  controls the way pictures are displayed. For the desktop picture, this
  dialog lets you modify a similar set of parameters for centered desktop
  pictures. In both cases, the options in this new sub-dialog do the same
  thing:

  \x3 Fit to screen: If this option is not checked, the picture is
    displayed at its original size. If it is checked, the picture is
    expanded (or shrinked) to the full size of the screen.

  \x3 Aspect ratio: Since you may sometimes need to use pictures that were
    originally taken from a video mode which uses a different shape of
    pixel (like viewing an ST Medium resolution picture in ST High) these
    options control the way this situation is corrected.

    \x3 Don't care: If "Fit to screen" is off, the picture is displayed
      without any resizing at all. If it is on, the picture is resized to
      the exact size of the full screen.

    \x3 Current screen: Any differences in aspect ratio are corrected.  For
      instance, to show a Degas PI2 in ST High resolution, the picture is
      expanded vertically by a factor of 2. Furthermore, if the picture is
      too large for the screen, it is shrinked.

    \x3 Picture: Use the original shape of the picture. If "Fit to screen"
      is off, the picture is displayed without any resizing at all.  If it
      is on, the picture is resized to the largest size that can fit onto
      the full screen, without changing the picture's aspect ratio.
      Regardless of the Fit mode, if the picture is too large for the
      screen, it is shrinked.

  Note that using these options can significantly increase the time it
  takes to load a picture, especially when "Fit to screen" is used and the
  picture also gets dithered.

Related topics:  \#Desktop Pictures\#  \#Viewing Files\#
\end

\screen Dialogs
\hidden Dialogs...
\screen Dialog preferences
Dialogs
-------

  Dialogs are used by NeoDesk whenever it needs to display and receive
  information from you. You can always get out of a dialog without changing
  anything by selecting the Cancel button, clicking on its \#close window\#
  button (if the dialog is being shown in a window), or by pressing the
  [UNDO] key.

  All NeoDesk dialogs have access to NeoDesk's built-in help system.
  Pressing the HELP button or pressing the [HELP] key will bring up
  specific help about the dialog you are in.

  NeoDesk dialogs also have easy to use keyboard equivalents. These are
  identical to those offered through the Geneva environment. A unique
  character in each button (or selectable item) is underlined. You can
  select that item by pressing the appropriate underlined key. If the
  dialog has a text entry field, you can still select the dialog items by
  holding down the [ALTERNATE] key while pressing the appropriate key.

  You can change the NeoDesk dialog settings by selecting the Dialogs...
  entry in the Settings desktop drop-down menu.

  By default, NeoDesk dialogs are set to "Display in a window". This means
  that you can easily move them around the screen, or even select other
  windows (while the dialog remains open).

  If you hold down either [SHIFT] key while selecting the Ok!  button in a
  windowed dialog (you can also press [SHIFT]-[RETURN]), the dialog's
  settings will be implemented, but the dialog will remain on the screen.

  Since there is a limit to how many windows can be open at any one time
  (seven on regular TOS, more if running software like Geneva or MultiTOS),
  NeoDesk will display the dialogs without a window whenever it runs out of
  available windows. However, some dialogs (such as the one used for
  Macros) will always be displayed in a modal dialog (without a window).

  You can also force all the dialogs to display without a window, by
  unselecting the "Display in a window" button.

  However, all NeoDesk dialogs can still be dragged, even when displayed
  without a window. This is done by using the upper right corner of the
  dialog, where a dragging symbol is displayed.

  You can also indicate the position where the dialogs will appear when
  they are displayed. This can be the "Last used" position or "Centered" on
  the screen. With the "Follow mouse" option, the dialogs will
  automatically appear centered under the current mouse position.

  If you are not using Geneva, then the \#Button Colors\# button will allow
  you to modify the colors of the buttons used in NeoDesk's dialogs.  If
  you have Geneva, then you can use the Task Manager's "Dialogs" option to
  change these settings.

Related topics:  \#Using NeoDesk\#
\end

\screen Print Directory...
\screen Directory printing options
Print Directory
---------------

  This entry will display a dialog that allows you to print the contents of
  the current window directory. These contents can be either sent to a file
  or printed to the printer. The dialog lets you choose if the printout
  should include the Size, Date, and Time of the window's items. In
  addition, the "Folder contents" option lets you also include the contents
  of all \#folders\#.

Related topics:  \#View Menu\#  \#Printer Icon\#
\end

\screen Copying Disks
\screen Disk copying options
\screen Disk Copy
\#Copying\# Disks
-------------

  When a drive icon is dragged to another drive icon, NeoDesk analyzes the
  disk types to see what kind of disk copy is possible. A special disk copy
  dialog will appear, which also shows how many disk swaps will be required
  for each type of copy operation. A disk swap is defined as being when
  NeoDesk goes back and forth between the source disk and the destination
  disk. The more free memory on your system, the fewer disk swaps will be
  required.

  If the "Copy \#in background\#" button is enabled, the disk copying will take
  place in the background.

  If you try accessing either the Source or Destination drives from inside
  NeoDesk while a disk copying operation is taking place, you will receive
  an error message. If you try doing this from inside another program, you
  will probably get either "Data on this drive may be damaged" or simply an
  empty directory. This is because NeoDesk is actively preventing the
  disks' contents from changing or being read during the operation.

  The Number of copies option lets you specify how many copies should be
  made of the source disk. Since NeoDesk usually reads the entire source
  disk into memory (as long as you have enough free memory), this becomes
  an easy way to make multiple copies of a single disk.

  There are three disk copying options available. If any option is not
  currently available, it will be grayed out:

  \x3 Files Only -  This option will copy all the files from the source disk
    to the destination disk, including all hidden and system files.  All
    existing data in the destination drive is preserved, and the success of
    the operation is based on there being enough available space on the
    destination drive for all the files to be copied.

  \x3 With Format - This option will analyze the size of the source disk and
    then format the destination disk to the same size while copying.
    However, it will make sure that the destination disk is formatted using
    the method set in the \#Miscellaneous...\# settings dialog in the Settings
    desktop drop-down menu. Finally, it will copy the source disk's entire
    contents over to the destination disk.

  \x3 No Format - This option requires that both the destination disk and the
    source disk have been previously formatted to the same disk size and
    type. All data on the destination disk will be overwritten.

    This option will also work with identical size hard disk partitions.
    If both hard disk partitions are exactly the same size and type, then
    NeoDesk will let you backup one hard disk partition to another.

Related topics:  \#Using NeoDesk\#
\end

\screen Disk operation status
Disk Operation Status
---------------------

  This dialog is shared by both the \#Format Floppy...\# and \#Disk Copy\#
  commands. Please refer to these sections for more information.
\end

\screen Icons
\screen Default Icons
\screen Drive Icons
Icons
-----

  NeoDesk's icons work in a very similar manner to those in the built-in
  desktop. You can select them, move them, and manipulate them just like
  you can manipulate icons in the built-in desktop.  NeoDesk allows you to
  customize and edit any of the icons it uses. It also makes it possible to
  drag icons out of \#windows\# and place them on the desktop. NeoDesk also
  introduces several new types of icons.

  Like the built-in desktop, NeoDesk has a set of default icons on its
  desktop. The drawer icon, which was used to represent drives, has been
  replaced with three new icons which represent a hard drive, a floppy
  disk, and a RAM disk. In addition, NeoDesk adds new printer and clipboard
  icons to the regular \#trashcan icon\#.

  The separate hard drive, floppy disk, and RAM disk icons work just like
  the built-in desktop's drawer icon. They can be assigned to any drive
  letter of your choice.

  You can modify, remove, or add drive icons to your desktop by selecting
  the \#Install Desktop Icon...\# entry under the Options desktop drop-down
  menu.

  Any drive icon can be selected or \#open\#ed (just as if you single or
  double-click on it) from the keyboard by holding down the [SHIFT] key
  while pressing the letter that drive. Pressing the icon's letter once
  will select that icon, a second time will open it. Thus, if you wanted to
  open a window to drive A, you would hold down the [SHIFT] key and press
  the "A" key twice.

Related topics:  \#Trashcan Icon\#  \#Printer Icon\#   \#Clipboard Icon\#
                 \#File Icons\#     \#Desktop Icons\#  \#Assigning New Drive Icons\#
\end

\screen Edit Icons...
Edit Icons
----------

  The Edit Icons... entry will load the NeoDesk Icon Editor. For more
  information on how to use the Icon Editor, consult your NeoDesk 4 owner's
  manual.

Related topics:  \#Options Desktop Menu\#
\end

\screen Change Item...
\screen Edit item in group
Edit Item in Group
------------------

  Change Item..., allows you to change the name and description of that
  item within a group. This does not affect the item's original filename.
  This special group name can be up to 20 characters long and include
  lowercase characters and spaces.

  You can define a keystroke to open an item in a group by using the
  Keyboard shortcut section. When the correct keys are pressed on the
  keyboard, the item in the group is opened, exactly as though it had been
  double-clicked on. Note that in order for this function to work, the
  group window containing the item must be open.

  This feature uses the same GRP file format that is employed by the Geneva
  Run program (which is included with Geneva 004 and newer.) Keyboard
  shortcuts defined by one program can be used by the other.

Related topics:  \#Options Window Menu\#  \#Opening Groups\#
                 \#New Group...\#         \#Show Info on Groups\#
\end

\screen Environment...
\screen Environmental variables
\screen Edit variable
Environmental Variables
-----------------------

  This dialog allows you to enter up to ten environmental variables, which
  are given to programs that are run by NeoDesk. While most programs ignore
  these, some do take advantage of them. For example, the popular ARC.TTP
  utility will look for a "TEMP=" variable to see which path it should use
  to store temporary files. Usually, if a program can use environmental
  variables, it will say which ones somewhere in its documentation.

  The "Use environment from parent" option will disable this feature and
  NeoDesk will pass on the environmental variables that were given to it
  when it first loaded.

Related topics:  \#Settings Menu\#
\end

\screen Erasing Disks
\hidden Erase
Erasing Disks
-------------

  Whenever you drag a drive icon to the \#trashcan icon\# (the built-in one),
  or use the \#Delete\# option in the desktop drop-down menu, you can delete
  the entire contents of the disk in one very fast operation. NeoDesk will,
  after warning you, zero out that disk's directory and its File Allocation
  Table (FAT).

  This is much faster than deleting every single file on the disk one by
  one. Any clusters in the FAT that are already marked as bad are not
  affected. You should use this feature carefully, since it is almost
  impossible to recover data off a disk that has been deleted using this
  method.

Related topics:  \#Using NeoDesk\#  \#File Desktop Menu\#
\end

\screen Executable Boot Sectors
Executable Boot Sectors
-----------------------

  If the disk that the window is displaying has an executable boot sector,
  a small "E" indicator will appear in the upper right hand corner of the
  area where the NeoDesk \#Volume Name\# is shown (at the bottom right of the
  window).

  This is very useful if you are looking for a computer virus. Most disk
  viruses are stored as executable boot sectors. While this does not
  indicate that you do have a computer virus, if you see it on a disk that
  should not have an executable boot sector you should probably investigate
  further.

  Note:  If you are using a hard drive, the boot partition (usually "C")
         will have an executable boot sector. This is normal. Also, many
         autobooting game disks will have one. This is normal and should
         not be confused with a virus.

Related topics:  \#Windows\#  \#Volume Name\#
\end

\screen Extensions...
\screen Executable filename extensions
Executable Filename Extensions
------------------------------

  The Extensions... entry allows you to modify, remove, or add executable
  filename extensions. It is through these extensions that NeoDesk decides
  what can be executed and what kind of executable it is. You can scroll
  through the installed extensions (using the arrow buttons), and easily
  create a New one or Delete an existing one. Under NeoDesk, there are
  three types of files that can be \#open\#ed:

  \x3 TOS - These are files that usually end in either ".TOS" or  ".TTP".
    They are usually text based programs that do not use the GEM portion of
    the operating system. With Geneva, MultiTOS, or the NeoDesk CLI, they
    can be run inside a window. Otherwise, they take over the entire screen
    until finished.

  \x3 GEM - These are files that usually end in ".APP", ".NPG", or ".GTP".
    They have the capability of taking full advantage of the GEM
    environment (though they don't always) and can use a graphic display.

  \x3 Batch - These are files that usually end in either ".BAT" or ".BTP".
    They are data or text files which are meant to be passed on to a
    separate \#Batch File\# Interpreter (also known as a Command Line
    Interpreter or CLI). This is usually specified in the \#Paths...\#
    settings dialog.

  These three types of executables can have two variations:

  \x3 Takes parameters - These are executables that need the Parameters for
    execution dialog. When NeoDesk tries to open an executable with this
    option, it will first let you enter any necessary parameters.

  \x3 NeoDesk program - These are special programs that have been written to
    hook directly into NeoDesk (through use of the NeoDesk Developer's
    Kit). An example of this is the NeoDesk CLI, which in addition to
    running as a desk accessory can also be run as a NeoDesk program. When
    NeoDesk opens an executable with this option, it gives it the necessary
    information it needs to hook directly into NeoDesk.

  NeoDesk ships with the following default extensions for executables:

  TOS     TOS program.
  PRG     GEM program.
  APP     GEM program.
  TTP     TOS Takes Parameters program. Uses \#Parameters for execution\#
          dialog. Identical to the built-in desktop.
  GTP     GEM Takes Parameters program. Uses Parameters for execution
          dialog. Introduced in more recent versions of the built-in
          desktop.
  NPG     NeoDesk program. Receives (as a parameter) information needed to
          properly hook into NeoDesk. Unique to NeoDesk.
  NTP     NeoDesk Takes Parameters program. Uses Parameters for execution
          dialog. Unique to NeoDesk.
  BAT     Batch file. The selected batch file interpreter is executed and
          the opened file is given to it in the form of a parameter.
          Introduced by NeoDesk.
  BTP     Batch Takes Parameters file.  Uses Parameters for execution
          dialog (the name of the batch file appears first, as it is passed
          as a parameter to the batch file interpreter). Introduced by
          NeoDesk.
  ACC     GEM program. This extension will only appear by default if you
          are running NeoDesk with either Geneva or MultiTOS (where a desk
          accessory can be executed as a program).

Related topics:  \#Settings Menu\#
\end

\screen Begin Macro
\screen End Macro
\screen Macro assignments
\screen Execute/Change Macro...
Macro Assignments
-----------------

  The Begin Macro entry will allow you to record macros. A macro is a
  recording of any series of menu, window, and icon based operations while
  at the NeoDesk desktop.

  When you first select this entry, a special "M" indicator will appear in
  the upper left corner of the screen, which indicates that you are in the
  macro recording mode. In addition, the menu entry will change to read
  "End Macro".

  Any operation from this moment on will be recorded. Selecting the End
  Macro entry will pause the recording of that macro and display the Macro
  assignments dialog.

  Here you can set the name and keyboard shortcut for that macro. This
  keyboard shortcut will supersede any other keyboard equivalents,
  including those used to \#open\# desktop \#icons\#. You can also see its size and
  how much space is left for additional macros in the current macro file.

  Since macros can be loaded and saved independently, you can have a nearly
  unlimited number of macros, limited to 32K worth of macros per file (you
  can easily record a macro that loads a macro file).

  The Read Key button will let you press the key combinations that you want
  to assign to that macro. Any combination of [CONTROL], [LEFT SHIFT],
  [RIGHT SHIFT], and [ALTERNATE] keys can be used. In addition, the keypad
  keys can be used separately of the regular keyboard.

  Selecting the Install button will end the recording of the macro and
  install it as part of the current macro set. The Remove button will end
  the recording and delete the recorded macro. The Cancel button lets you
  return to the macro recording mode and continue recording the macro.

  Any of the operations on a window button will only play back if there is
  a window open to the same path as the window that the macro was recorded
  on. If there is more than one window open to the same path, the macro
  runs on the topmost window which matches that path.

  Any program that normally opens a \#Parameters for Execution\# dialog box
  will not open one when executed from a macro.

  You can stop macro playback by holding down both [SHIFT] keys at the same
  time.

  The macro function has one key limitation. It cannot record anything that
  happens inside a dialog , alert, or program. If you create a macro that
  renames a file, the macro will call the rename box, but you will still
  have to manually enter the new filename.

  Macro files can be saved and loaded using the \#Save Configuration\#... and
  \#Load Configuration\# commands. They are saved with their own ".MAC" file
  ending.

  You can easily record a macro that loads another macro file.  NeoDesk
  will recognize this action while recording a macro and not actually load
  the file. However, when you play back the macro, it will load the new
  macro file into memory. Macros can also be used in the same way to load
  different desktop information or desktop note files.

  The Execute/Change Macro... entry also calls the Macro assignments
  dialog. By selecting the scroll arrows you can view all the installed
  macros. You can also change the name and keyboard shortcut for any of the
  installed macros, or even remove them.

  In this case, the Run button is active, allowing you to execute any of
  the installed macros.

Related topics:  \#Options Desktop Menu\#
\end

\screen File Desktop Menu
File Desktop Drop-Down Menu
---------------------------

  The File desktop menu contains the following entries:

    \#Open\#
    \#Show Information...\#
    \#Quick Disk Info...\#
    \#Search...\#
    \#Delete\#
    \#Format Floppy...\#
    \#Print Desktop\#
    \#Form Feed Printer\#
    \#Quit NeoDesk\#

Related topics:  \#Desktop Drop-Down Menu\#
\end

\screen File Icons
File Icons
----------

  NeoDesk also uses icons to represent different files, \#folders\#, and groups
  of files. These are the icons that you see whenever you \#open\# a file
  window. There are special icons to indicate if an item is a folder,
  program, \#batch file\#, other file, or group. If you want to view your files
  as text, you can select the \#Show as Text\# command from the View window
  drop-down menu.

  NeoDesk also supports color icons. The number of colors available depends
  on the type of screen display you are using. Basically there are four
  kinds of icons: two color, four color, and 16 color icons.

  Unlike the built-in desktop, NeoDesk allows each icon to have a different
  view when it is selected. Therefore, each icon actually consists of two
  icons (one view for when it is unselected and another for when it is
  selected). These can be easily edited using the Icon Editor.

Related topics:  \#Icons\#  \#Custom Icons\#  \#Folders\#  \#Groups\#
                 \#Desktop Icons\#
\end

\hidden On Files
\screen File information
Show Info on Files
------------------

  When you Show Information on a file, you can view and change its
  filename. You are also shown the file's length in bytes and the time and
  date it was last modified. By selecting the Set date and time to right
  now button, you can set the file's modification date and time to the
  current system date and time. This is also known as a "touch" command.

  The file's read/write status can also be determined and changed. Normally
  the file is in "read and write" mode, which allows for the file to be
  read, modified, or even \#delete\#d. If the "Read only" button is set, the
  file is write protected. This prevents the file from being modified or
  deleted.

  You can also set the program's Fastload status. This feature only works
  on machines equipped with TOS 1.4 or newer. Whenever it is turned on, the
  memory past where the program ends will not be cleared when it runs,
  resulting in slightly faster loading time. You should be warned that some
  programs, especially older ones, expect the memory to be cleared and
  therefore are not compatible with this feature.

  If you are using a TT/030, Falcon030, or an ST with "TT" style memory,
  you can specify if the program should "Load into TT RAM" and/or whether
  it should "Allocate TT RAM". These program flags are ignored on regular
  ST systems.

  Users of the MiNT multitasking kernel can also set the Private, Global,
  Super, and Read only flags. Please refer to your MiNT documentation for
  more information. These flags are ignored if you are not running MiNT

Related topics:  \#File Desktop Menu\#  \#Show Information...\#
\end

\screen In background
\screen File...
\screen File preferences
File Preferences
----------------

  In the File preferences dialog, you can specify a number of file related options:

  \x3 Confirm copies/moves - When this option is selected, NeoDesk will
    confirm all file copy and move operations with you before performing
    those operations. Initially, this option is turned on.

  \x3 Confirm \#delete\#s - When this option is selected, NeoDesk will confirm
    all file deletions. Initially, this option is turned on.

  \x3 Confirm overwrites - When this option is selected, NeoDesk will confirm
    that you really want to overwrite an existing file when you copy or
    move a file with the same name. Initially, this option is turned on.

  \x3 Pause after TOS programs - When running without Geneva or MultiTOS,
    this option will pause the display after a ".TOS" or ".TTP" text based
    program has finished running. This allows you to read a program's final
    display before returning to the desktop. Initially, this option is
    turned on.

  \x3 Unload NeoDesk for execute - When this option is selected, NeoDesk will
    remove itself from memory whenever it executes a program (saving quite
    a bit of memory for program execution). As soon as it regains control,
    it will load itself back into memory.

    This feature is intended for running NeoDesk in a singletasking
    environment. When running in a multitasking environment, NeoDesk
    regains control instantly after running a multitasking program.  The
    result is that NeoDesk immediately reloads itself after running a
    program. Most likely, you will want to keep this option off if you are
    running inside a multitasking environment.

  \x3 Alert before quitting - When this option is selected, NeoDesk will
    issue an alert before actually quitting (either through your selection
    of the Quit command or because NeoDesk received a message from the
    operating system telling it to quit). Initially, this option is turned
    on.

  \x3 Default icon dragging mode - Here you can select the default mode that
    NeoDesk will use when you drag an icon from one location to another.
    When \#copying\#, the mouse remains in a regular arrow shape to indicate
    that the items being dragged will be copied. When moving, the mouse
    changes to the hand shape to indicate that the items being dragged will
    be moved. Initially, this option is set to Ask, where NeoDesk will ask
    you what kind of operation you desire.

    If you have a default mode set (Copy or Move), you can toggle to the
    opposite operation by holding down the [CONTROL]-[LEFT
    SHIFT]-[ALTERNATE] keys at the same time as you are dragging the items
    to be copied or moved. To show which option is currently selected, the
    \#mouse pointer\#'s shape will toggle between the regular arrow shape
    (indicating copy) and the hand shape (indicating move).

  \x3 Bad files into clipboard - This option specifies what NeoDesk should do
    when it encounters a file error while performing a file copy or move
    operation. If the Always option is set, NeoDesk will copy the files
    that it was able to successfully read into the File Clipboard.  The
    Never option turns this feature off. Initially this option is set to
    Ask, which lets you choose what to do when a file error is encountered.

  \x3 Background operation speed - This option specifies how fast background
    file and disk operations should proceed. The faster they go, the slower
    the rest of the system will operate. Therefore, if you are running a
    slow system, you should choose the slower settings. If you are running
    a fast system, you can then choose one of the faster settings. You may
    have to try several variations to find which setting works best on your
    system.

Related topics:  \#Settings Menu\#  \#Copy/Move Items\#  \#Format Floppy...\#
\end

\screen File Window Menu
File Window Menu
----------------

  The File window drop-down menu contains the following entries:

    \#Open\#
    \#Show Information...\#
    \#Quick Disk Info...\#
    \#Search...\#
    \#Delete Item\#
    \#Create Folder...\#
    \#Close Folder\#
    \#Close Window\#
    \#Select All Items\#
    \#New Group...\#

  A group menu also has a \#Save Group\# option.

Related topics:  \#Window Drop-Down Menus\#
\end

\screen Filtering Items
\screen Search filter
\screen File operation filter
\screen Directory filter
\hidden Filter...
\screen Filter
Filtering Files & Folders
-------------------------

  This one dialog can be used for several different purposes, either to
  choose which items are currently visible, to choose which items to copy
  or move, or to search for certain items.  The type of operation that the
  dialog is being used for will usually be reflected in its name.

  This dialog has a number of different options. You can individually set
  the file \#Templates\#, Size, Date, Time, and Attributes. Depending on the
  type of operation, there is a sixth option that applies only to searching
  (Create group) and filtering (All folders). This option changes
  automatically depending on the operation you are performing.

  These options have an additive effect. In other words, the actual
  file/folder filtering will be performed using all the options selected.

  You can also Load and Save filter sets that you might want to use at a
  later time. These are saved in special ".NFL" files. This lets you save
  complex filtering sequences that you might want to use at a later time.

  The Save option does not save the default settings for this dialog. The
  default settings are saved when you select \#Save Configuration\#... from the
  Options desktop drop-down menu and save your desktop configuration file
  (.INF).

  \x3 Size

    You can specify the size range of the files you are attempting to
    filter.  By selecting the appropriate button, you can specify if the
    file is larger, smaller, or equal to a certain size.

  \x3 Date

    The date is specified in the same way, by entering a date that the file
    would greater than (newer), lesser than (older), or equal to. The date
    format is how it is specified in the \#Miscellaneous...\# settings.

  \x3 Time

    Time is selected in the same manner as Date. The time is entered in
    24-hour format (times after 12:00 PM are calculated by adding 12 to
    them).

  \x3 Attributes

    The Read-only attribute indicates if a file has been set to read-only
    mode (this can be done using the \#Show Information...\# command in the
    File drop-down menu). The Archive attribute indicates if a file has had
    its archive attribute set, which is usually only meaningful to
    specialized backup software.

Related topics:  \#Searching Files\#  \#Templates\# \#Copying/Moving Items\#
                 \#Deleting Items\#   \#File Window Menu\#
\end

\screen Folders
Folders
-------

  With folders you can collect together multiple related files. Folders can
  also hold other folders. This becomes a very easy way to organize your
  disks and keep track of all your programs and files.

Related topics:  \#Icons\#  \#File Icons\#
\end

\screen For your information
Queued Operations
-----------------

  NeoDesk file and folder operations operate through a queue. If you
  attempt to copy, move, or delete additional items while a similar
  operation is already taking place, your request will be placed in the
  queue. As soon as the current operation is completed, the \#Copy/Move items\#
  dialog will reappear ready for the next operation.

Related topics:  \#Copy/Move Items\#  \#Delete Item\#
\end

\screen Update Window
\screen Force Update
Update Window
-------------

  This entry will update the window contents by re-reading the information
  from the disk.

Force Update
------------

  This entry, while similar to the Update Window command, will force a
  "media change" before re-reading the information from the disk. This
  special call ensures that the system re-reads the contents of the new
  disk (instead of the contents of a cache in memory). In some cases and in
  some floppy drives, this is the only way to get the system to re-read the
  disk contents correctly.

  The "media change" fools the system into thinking that there is a new
  disk in that drive (even if it is a hard disk). You should be careful
  when using this in a multitasking system, as any open files (files being
  used by running programs) will also be closed, causing potential
  problems.

Related topics:  \#View Menu\#
\end

\screen Form Feed Printer
Form Feed Printer
-----------------

  The Form Feed Printer entry will send a form feed character, which
  usually ejects a page, to your printer. This is especially useful with
  laser printers, which will not print until they have received a form feed
  character.

Related topics:  \#File Desktop Menu\#  \#Printer Icon\#
\end

\screen Formatting Disks
\screen Floppy disk formatting options
\screen Format Floppy...
Formatting Disks
----------------

  The Format Floppy... entry will allow a floppy disk to be formatted.
  First select the floppy drive you want to format, then select the Format
  Floppy... entry in the File desktop drop-down menu.

  Disk: You can format disks in either the "A" drive (the regular default
  drive that is comes with most Atari computers) or the "B" drive (usually
  an second disk drive).

  Sides: Depending on the type of disk drive you have, you can format
  single sided or double sided disks. Some of the earlier ST computers only
  came with single sided disk drives and therefore are unable to read or
  format double sided disks.

  Sectors: These are the number of sectors per track. The 9 sector per
  track option is the one used by the built-in desktop. The 10 sectors per
  track option, however, can hold more data.

  Tracks: This indicates how many tracks will be placed on each side of
  the disk.  The 40 track option is designed for 5 1/4-inch size drives. If
  you set a disk drive's size in the NeoDesk Control Panel to 5 1/4-inch,
  this becomes the default option for that drive. The 80 track option is
  standard for most disks. Formatting any floppy disk beyond 80 tracks is
  not standard and thus not considered completely reliable, though many
  people have reported no problems in going beyond this limit.

  Type: Over the years several new ways to format a disk have been
  developed.  These are faster than the Normal method offered by the
  built-in desktop.  Twister is the faster option for 10 sector disks. Fast
  11 is the faster option for 9 sector disks. If you want, NeoDesk will
  automatically choose the faster option (Auto), depending on the number of
  sectors selected.

  Density: If your floppy drive is capable of formatting 1.44 megabyte
  High density diskettes or 2.88 megabyte Extended diskettes, NeoDesk will
  make these options available.

  Sectors/cluster: There is also a choice of using 1 or 2 sectors per
  cluster. Using 1 sector will result in less wasted space on the disk, but
  can also result in less free space to start with. On a PC, HD floppy
  disks are formatted with 512-byte clusters (1 sector/cluster), and
  regular disks use 1024-byte ones (2/cluster), so these are the default
  values. The sectors per cluster are stored independently for each disk
  density, and will switch as you choose a new density.

  Note: Some versions of MS-DOS may not be able to read disks formatted
  using 20 sectors/track. Use 19 for best compatibility.

  If the "Format \#in background\#" button is selected, NeoDesk will format the
  disk in the background (a multitasking environment is not required).  The
  main drawbacks are speed and system slowdown. The floppy (since it is now
  formatting with a lower system priority) takes longer to format.  Also,
  the entire system will slow down (though you can specify by how much the
  system should slow down while formatting a floppy in the File...
  settings dialog in the Settings desktop drop-down menu).

  Once the formatting process starts, you can easily toggle the background
  formatting process with the "In background" button. Turning off
  background formatting will speed up the formatting process.

  There's also a Pause button, which lets you temporarily stop the
  formatting process. You can actually stop a disk in the middle of a disk
  format, take it out, use the drive for read a file from another disk, put
  it back in and finish formatting it.

  You should be very careful when using the pause control. If you put in
  the wrong disk when starting back up, it will be destroyed.  Also, a
  keeping partially formatted disk is basically useless, since it has to be
  reformatted.

  If you try accessing the drive being formatted from inside NeoDesk, you
  will receive an error message. If you try doing this from inside another
  program, you will probably get either "Data on this drive may be damaged"
  or simply an empty directory. This is because NeoDesk is actively
  preventing the disks' contents from changing or being read during the
  operation.

Related topics:  \#Using NeoDesk\#
\end

\screen Window Full Size Button
\screen Full Size
Window Full Size Button
-----------------------

  The full size button, at the upper right of the window, will force the
  window to grow to the full size of the desktop. Selecting the button a
  second time will return the window to its original size.

  NeoDesk also allows you to adjust the full size that the window grows to
  whenever you select the full size button. Once you have made a window the
  size of the desktop, you can resize it to a new location and size. From
  that point on, selecting the full size button will make the window jump
  back and forth between its original location and size to the new location
  and size.

Related topics:  \#Windows\#
\end

\screen Gribnif Software
Gribnif Software
----------------

  You can write to us at:
    Gribnif Software
    P.O. Box 779
    Northampton, MA  01061-0779

  You can call our technical support line at (413) 247-5620 between 10am
  and 6pm weekdays (Eastern Time). You can also send us a fax at
  (413) 247-5622.

  We are also available on several online networks. On GEnie you can send
  electronic mail to our "GRIBNIF" address. On CompuServe you can send
  electronic mail to our "75300,1131" address. Users of other online
  networks can send mail via Internet to "gribnif@genie.geis.com".
\end

\screen Help File Menu
Help File Menu
--------------

  The File drop-down menu provides a way to open other help files and
  perform other file operations on the current help file.

  \x3 Open Help File - This option selects a new help file to view. The Help
    Viewer is capable of reading files created by Geneva's own help text
    compiler and files in Pure C's help file format.

  \x3 Set Help File Path - This option allows you to select a path which
    contains other help files. These files are searched when a particular
    topic is not found in the current help file. This feature is not needed
    for NeoDesk's own help, but might be used if you are using other help
    files, especially if you want the \#Help Viewer\# to automatically search
    the contents of those files.

  \x3 Copy to Clipboard - You can select a block of help text with the mouse
    and save it to a clipboard file. Other programs which support the Atari
    clipboard (like STalker 3) can then paste this information into their
    own documents.

  \x3 Save Text - This option will save the current help screen, in its
    entirety, to a text file.

  \x3 Save Block - This option will save to disk the currently selected
    block.

  \x3 Quit - This option quits the \#Help Viewer\#.

Related topics:  \#Help Viewer Menus\#
\end

\screen Help Settings Menu
Help Settings Menu
------------------

  The Settings drop-down menu lets you change both the size of the Help
  Viewer's text font and, if any version of GDOS is loaded, the font
  itself. Any monospaced font can be used, in any of its sizes. The help
  viewer's settings are saved using the \#Save Configuration\#... command
  within NeoDesk. Font changes take effect immediately.

Related topics:  \#Help Viewer Menus\#
\end

\screen Online Help Viewer
\screen Help Viewer
Online Help Viewer
------------------

  NeoDesk 4 includes a built-in Help facility with hypertext links. It is
  available whenever NeoDesk is active and is called by either pressing the
  [HELP] key or by clicking on any of the HELP buttons in any of the
  NeoDesk \#dialogs\#.

  Since the Help is context-sensitive, it will come up displaying
  information about the dialog from which you called for help. If you press
  the [HELP] key when no dialogs are open, it will display the topmost
  level of the online help.

  If you are using Geneva, the Geneva Help Viewer will appear whenever you
  call for help. Without Geneva, NeoDesk will use its own internal Help
  Viewer (which is identical to the Geneva Help Viewer).

  Once you have opened the Help Viewer (which appears in a window), you can
  resize its window like any other window. If you are running in a
  multitasking environment, you can leave it on the screen for reference at
  any time.

  If you call for help from inside a non-windowed dialog, the Help Viewer
  will appear in the form of a dialog, instead of a window.  That means
  that you cannot resize it or access other windows (until you close it).

  The Help Viewer displays the help information with certain key words or
  phrases underlined. These are alternate topics (hypertext links), which
  can be referenced by double-clicking on them with the mouse.  This takes
  you directly to that topic.

Related topics:  \#Help Viewer Menus\#  \#Help Viewer Status Line\#
\end

\screen Help Viewer Menus
Help Viewer Menus
-----------------

  The Help Viewer has three drop-down menus of its own. These let you
  control a number of its features:

    \#Help File Menu\#
    \#Help Topic Menu\#
    \#Help Settings Menu\#

Related topics:  \#Online Help Viewer\#
\end

\screen Help Viewer Status Line
The Help Viewer Status Line
---------------------------

  The Help Viewer's status line, below the title bar, shows an updated
  display of your current location in the help text. Categories and
  sub-categories you have seen are shown from left to right. This provides
  a quick visual reference, and indicates what level you'll go back to if
  you choose the \#Previous\# command or press the [UNDO] key.

Related topics:  \#Online Help Viewer\#
\end

\screen Independent Windows
Independent Windows
-------------------

  Each NeoDesk window can be set to its own View and Sort options. Each
  window's View and \#Sort menu\# options changes its own settings. The
  settings of the topmost window are used every time a new window is
  opened.  For example, a window can be set to display text while another
  displays icons. At the same time, a window can be set to display all
  files sorted by name and another window to display all files sorted by
  size.

Related topics:  \#Windows\#
\end

\screen Input/Output Redirection
Input/Output Redirection
------------------------

  Input and output redirection is supported for text based ".TOS" & ".TTP"
  programs (GEM programs do not support this). This is done by entering the
  following information into the \#Parameters for execution\# dialog, to the
  right of any program parameters:

     > filename     - Redirect output to a new specified file.
     > PRN:         - Redirect output to the printer.
     >> filename    - Redirect to the end of an existing file.
     < filename     - Redirect input from the specified file.

Related topics:  \#Open\#  \#Parameters for Execution\#
\end

\hidden Install Application...
\screen Install application
Install Application
-------------------

  This menu entry lets you install an application so that you will be able
  to \#open\# one of the file endings installed for that application. When
  you do this, NeoDesk will automatically run the installed application and
  give it the file you tried opening as a parameter. If the application
  supports this feature, it will then proceed to load that file.

  To install an application, first select a program from a window or the
  desktop, then select this entry. The dialog will appear where you can
  install the file extensions for that program. Extensions must be
  separated by a comma, and the extended \#Wildcard characters\# are
  supported here. For instance, if you have a file viewer that can show
  Degas and Neochrome pictures, you might enter:

    NEO,P[IC][1-3]

  You can change or remove any already installed application by selecting
  this entry without first selecting an icon. The dialog box will appear,
  and by using the scroll arrows you will be able to see all of your
  \#installed applications\#. You can modify their settings and remove any
  unwanted ones.

  Once you have installed the application, opening any document ending with
  those extenders will result in the appropriate application being opened
  with the file you selected as a parameter. When you install an
  application, there are several options available:

  \x3 Program type - This option specifies how the program should be treated.
    The TOS, GEM, and Batch buttons let you specify if the program is a TOS
    based program, GEM based program, or a \#batch file\#.  The "Takes
    parameters" button specifies if this program needs to receive
    parameters and therefore the \#Parameters for execution\# dialog will be
    opened whenever it is run. The NeoDesk program button specifies that
    the program is designed to hook directly into NeoDesk. These options
    override the \#Extensions...\# defaults in the Settings desktop drop-down
    menu.

  \x3 Unload for exec - Here you can specify if NeoDesk should always unload
    itself from memory when you run that program or remain resident in
    memory. The Default option specifies that the setting in the \#File...\#
    dialog in the Settings desktop drop-down menu should be used.

    If you are running a multitasking system, you should only use this
    option if the program you are installing is a singletasking
    application. Otherwise, NeoDesk will instantly load back into memory
    once it executes the program.

  \x3 Set path to... - This is where NeoDesk sets the system path to when
    running an installed application. The default option sets the system
    path to that of the program being run. Most applications work fine with
    this method. The second choice sets the path to that of the document
    being opened, which is identical to the method used in older versions
    of the built-in desktop. This second option can be used when you
    discover one of the few programs that do not work correctly when the
    system path is set to that of the program being run.

Related topics:  \#Options Desktop Menu\#
\end

\screen Assigning New Drive Icons
Assigning New Drive Icons
-------------------------
  
  A new feature in the icon editor allows you to create custom icons for
  individual disk drives. To assign a new icon, do the following in the
  icon editor:

    1. Open the NeoDesk Icons window.
    2. Create a new custom icon, either by copying it from a NIC file, or
       by using the Create Icon menu option.
    3. Open the new icon.
    4. Select the "Type" popup. (It should already say "File", "Folder",
       or "Both".) A new option, Drive, is here in this popup. Choose it.
    5. The icon's template will change to "Drive _:". Enter the letter of
       the disk drive for which you want to use this icon. For instance,
       to use this icon for Drive C:, type the letter C.
    6. Select the OK button in the editor. The drive icon on the desktop
       should immediately update to reflect the change you just made.

  Additional note: When using automatic reorder mode in the icon editor,
  disk drive icons are always placed toward the beginning of the list of
  icons in the NeoDesk Icons window.

Related topics:  \#Icons\#  \#File Icons\#  \#Custom Icons\#
                 \#Install Desktop Icon...\#
\end

\screen Install Desktop Icon...
\screen Install/remove desktop icon
Install/Remove Desktop Icon
---------------------------

  This dialog lets you install, change, or remove a desktop icon. It does
  not require that an icon first be selected to work.

  To install a new desktop icon, enter the name and letter of the icon (no
  drive letter is needed if installing a printer, trashcan, or clipboard
  icon), then point to the icon to install. Hold down the left mouse button
  and drag out of the dialog the icon you want.

  If you use volume names on your disk drives, you may want to choose the
  "Use Volume Name" option. When checked, NeoDesk will update the icon on
  the desktop to have the actual volume name of the disk in the drive. If
  the disk does not have a volume name, the default name for that type of
  icon appears. The update only occurs when NeoDesk first runs and when you
  update or open a window to that drive. There is no way for NeoDesk to
  know that you have put in a new disk otherwise. Another disadvantage to
  this option is that there must actually be a disk in the drive when
  NeoDesk first runs, otherwise you will get a "Drive is not responding"
  alert.

  All drive letters between A and Z are fully supported. The standard
  system limit for drives is still up to the letter P. The extra letters
  from Q to Z were added to support newer file systems. To install a
  specific icon for a specific drive, see the topic
  \#Assigning New Drive Icons\#.

  To install a new file, program, group, or folder desktop icon, drag the
  icon from an open window onto the desktop and release it.

  By first selecting one or more desktop \#icons\# and then this menu entry,
  you can change their names and letters. If the icon selected is a file,
  program, group, or folder icon, the new name will not affect the original
  filename.

  The "Install all available devices" button will install an icon for every
  available drive on your system. This is useful if you are using a
  removable hard disk, have re-partitioned your hard disk, or have added a
  hard disk to your system.

Related topics:  \#Options Desktop Menu\#  \#Assigning New Drive Icons\#
\end

\screen Installed Applications
Installed Applications
----------------------

  One way to run programs is to install them as Installed Applications.
  This is done by using the \#Install Application\#... command under the
  Options drop-down menu. When you install an application, you assign it a
  set of endings which are exclusive to that program. Whenever you \#open\# or
  double-click on a file with one of these file endings, the installed
  program will run with the file you tried opening as a parameter.

Related topics:  \#Open\#
\end

\screen Keyboard Control
Keyboard Control
----------------

  While NeoDesk is mostly designed for mouse control, it also has a
  complete new series of features aimed at making keyboard usage easier.
  You can now easily move around a window, find and select an item, and
  even \#open\# it right from the keyboard. Other keyboard commands let you
  scroll a window, and perform some basic file operations.

  Most operations in NeoDesk can be aborted by pressing the [UNDO] key, by
  holding down both [SHIFT] keys, or with the [CONTROL]-[C] key
  combination.

Related topics:  \#Windows\#  \#Window Display Area\#
                 \#Scrolling With the Keyboard\#
                 \#Selecting Items With the Keyboard\#
                 \#Searching for Items With the Keyboard\#
\end

\screen Keyboard Shortcuts
Keyboard Shortcuts
------------------

\x3 General Operations
  [LEFT SHIFT]-[RIGHT SHIFT]                        Abort current operation
  [CONTROL]-[C]                                     Abort current operation
  [UNDO]                                            Abort current operation
  [RIGHT SHIFT] key                   Don't de-select \#icons\# after operation
  [CONTROL]-[1] through [0]                     \#Open\# NeoDesk desk accessory
  [CONTROL]-[-]                                  Open NeoDesk Control Panel
  [CONTROL]-[=]                                  Open NeoDesk Printer Queue
  [CONTROL]-[LEFT SHIFT]-[ALTERNATE]-keypad [.]          Reset mouse on/off
  [CONTROL]-[LEFT SHIFT]-[ALTERNATE]-keypad [*]         Reset screen colors
  [CONTROL]-[LEFT SHIFT]-[ALTERNATE]-[ENTER]   wind_update(END_UPDATE) call
  [CONTROL]-[B]                                 Open \#batch file\# interpreter

\x3 Desktop
  [a] through [z]                 Select/open desktop icon with this letter
  [1] through [0]                 Select/open desktop icon with this number
  [SHIFT]-[A] through [Z]                            Select/open disk drive
  [CONTROL]-left mouse click and hold                      View item's path
  [CONTROL]-double mouse click               Open new window to item's path
  [CONTROL]-left mouse click on the desktop window close gadget
                   Close all NeoDesk windows & cancel any active operations

\x3 Desktop Notes Editing
  double mouse click on desktop                  Enter desktop notes editor
  [ESC]                                                          Clear line
  [left arrow]                                             Move cursor left
  [right arrow]                                           Move cursor right
  [SHIFT]-[left arrow]                         Move cursor to start of line
  [SHIFT]-[right arrow]                          Move cursor to end of line
  [BACKSPACE]                            Delete character to left of cursor
  [DELETE]                              Delete character to right of cursor
  [RETURN], [ENTER]                                                End edit

\x3 Opening Icons
  [ALTERNATE]                                Show file instead of executing
  [ALTERNATE]-[LEFT SHIFT]          Toggle Parameters dialog (open program)

\x3 Viewing a Text File
  [CONTROL]-[Z]                                           Toggle pagination
  [CLR/HOME]                                              Beginning of file
  [up arrow]                                                    Up one page
  [down arrow], [SPACEBAR]                                    Down one page
  [RETURN], [ENTER], [CONTROL]-[M]                            Down one line
  [CONTROL]-[C], [UNDO], [ESC], [Q]                                     End

\x3 Dialogs
  [RETURN]                                   Select the default exit button
  [ALTERNATE]-[TAB]         Cycle the default button among all exit buttons
  [SHIFT]-single mouse click on OK button   In a windowed dialog, apply the
                                          changes but leave the dialog open
  [UNDO]   Selects any exit button labeled Cancel, Quit, Exit, Abort, or No
  [ALTERNATE]-[underlined character] Selects the button with that character
  [TAB]                     Move the text cursor to the next editable field
  [down arrow]              Move the text cursor to the next editable field
  [SHIFT]-[TAB]         Move the text cursor to the previous editable field
  [SHIFT]-[up arrow]    Move the text cursor to the previous editable field
  [left arrow]              Moves the text cursor one character to the left
  [SHIFT]-[left arrow]      Moves the text cursor to the leftmost character
  [CONTROL]-[left arrow]         Moves the text cursor one word to the left
  [right arrow]            Moves the text cursor one character to the right
  [SHIFT]-[right arrow]      Moves the text cursor all the way to the right
  [CONTROL]-[right arrow]       Moves the text cursor one word to the right
  [ESC]                                                 Deletes entire line
  [SHIFT]-[DELETE]                           Deletes to the end of the line
  [SHIFT]-[BACKSPACE]                  Deletes to the beginning of the line
  [INSERT]            Toggles the cursor between insert and overwrite modes
  [ALTERNATE]-ASCII value         Enters character for ASCII value typed in
  [CLR/HOME]               In a non-widowed dialog, displays the full Atari
            character set, allowing you enter a character by clicking on it

\x3 Windows
  [CONTROL]-[CLR/HOME]                                 Go to root directory
  [ALTERNATE][\\]                                       Go to root directory
  [CLR/HOME]                                                  Scroll to top
  [SHIFT]-[CLR/HOME]                                       Scroll to bottom
  [up arrow]                                             Scroll up one line
  [down arrow]                                         Scroll down one line
  [SHIFT]-[up arrow]                                     Scroll up one page
  [SHIFT]-[down arrow]                                 Scroll down one page
  [CONTROL]-left mouse click                                    Rename item
  [CONTROL]-[SHIFT]-left mouse click                  Rename multiple items
  [CONTROL]-left mouse click on close box                      \#Close window\#
  [CONTROL]-double mouse click         Open new window to group item's path
  [CONTROL]-double mouse click on folder        Open folder in a new window

\x3 File Search Mode
  [TAB]                             Enter search mode/switch to next window
  [RETURN], [ENTER]                                               Open icon
  [BACKSPACE]                                            Back up one letter
  [UNDO]                                                   Clear the string

\x3 Copy/Move/Delete
  [CONTROL]-[SHIFT]-[ALTERNATE]                  Toggle between Copy & Move
  [SHIFT]                     Toggle wait for confirmation of the operation
  [CONTROL]                           Temporarily toggle Confirm Overwrites
\end

\screen Small Text
\screen Large Text
Small/Large Text
----------------

  This entry (it is a toggle) will let you choose which text size should be
  used when the \#Show as Text\# option is enabled. The actual fonts (and
  their sizes) used for this entry are set in the \#Windows...\# dialog in
  the Settings desktop drop-down menu.

Related topics:  \#View Menu\#
\end

\screen Load Configuration
Load Configuration
------------------

  The Load Configuration entry will load any of your saved desktop
  information files, macro files, or desktop note files.

  To use this feature, select a file ending in ".INF" (for desktop
  information files), ".MAC" (for macro files), or ".NOT" (for desktop note
  files). Then select the Load Configuration entry to load that file.

  While it is possible to load a desktop information file created in
  another screen resolution, it is probably best that you use the NeoDesk
  Information File Loader to convert the information file to the current
  resolution.

Related topics:  \#Options Desktop Menu\#  \#Desktop Notes\#
                 \#Macro assignments\#     \#Reload .INF\#
                 \#Save Configuration\#
\end

\screen Loading for the First Time
Loading NeoDesk for the First Time
----------------------------------

  When NeoDesk loads and runs for the first time (in that specific
  resolution), it will inform you that it cannot find a default Desktop
  Information File (.INF) and therefore it will run with its default
  settings. This is because no file has been created yet.

  Once you select the \#Save Configuration\#... entry under the Options desktop
  drop-down menu, NeoDesk will save a Desktop Information File. You will
  then have a saved desktop for that resolution and this message will no
  longer appear.

  Note:  When NeoDesk loads for the first time it checks the "_IDT" system
         cookie to decided which format to use for date displays (European
         or American). If you want to change the default settings, use the
         Miscellanous... dialog under the Settings desktop drop-down menu.

Related Topics:  \#Using NeoDesk\#
\end

\screen Memory...
\screen Memory usage
Memory Usage
------------

  The Memory usage dialog shows you the current system conditions. You can
  see how much total "ST RAM" is installed into your system and how much
  "alternate RAM" (also known as "TT RAM"). You can see how much of that
  memory is still available for use, in how many blocks, and the size of
  the largest block.  The more blocks your free memory is divided into and
  the smaller the largest block becomes, the more fragmented your system
  memory is (which is cleaned up by restarting your system).

  This dialog also displays the current ROM version, GEMDOS version, and
  AES version in your machine.

  The NeoDesk portion of this dialog controls how NeoDesk itself uses the
  available memory.

  The "Limit memory to xxx Kb" option tell NeoDesk to take at most the
  amount of memory entered here. The memory is requested from the system as
  needed, up to this limit. If you try to perform a operation that requires
  more memory than is available, NeoDesk will present an alert box asking
  for permission to go over this limit. This mode is best for systems with
  small amounts of free memory.

  The "Take xxx Kb at all times" option tells NeoDesk to always take a
  block of memory of the size specified here. This avoids memory
  fragmentation, but requires that the block remain allocated at all times
  (potentially using more memory than necessary). This mode is preferable
  for systems with TOS versions older than 1.4 (1.0 and 1.2), as they are
  more prone to memory fragmentation problems.

  The "No limit" option tells NeoDesk that it can freely allocate memory as
  needed. This can lead to fragmentation, but is the most efficient method.
  It also provides for much larger \#copying\# buffers than either of the other
  two methods. However, it is probably not a good idea to use this method
  with TOS 1.0 or 1.2.

Related topics:  \#Settings Menu\#
\end

\screen Paths...
\screen Miscellaneous paths
Miscellaneous Paths
-------------------

  The "\#Batch file interpreter\#" lets you select what program NeoDesk
  should execute whenever you double-click on a \#batch file\# (.BAT or .BTP)
  or you press [CONTROL]-[B]. You must enter its complete path and
  filename.  Whenever you try \#open\#ing a batch file, NeoDesk will run this
  program and give it the path and filename of the batch file you tried
  opening.

  The "Program to auto-execute" lets you specify the path and filename of
  any program that you want executed automatically the instant that NeoDesk
  finishes loading. This command is of limited use if you have Geneva or
  MultiTOS (as you would set any programs you want to run automatically
  using the GEM.CNF file).

  The "Alternate file viewer" entry lets you specify the path and filename
  of the file viewer that NeoDesk should use instead of its own built-in
  one.  Whenever you try opening a file to be viewed NeoDesk will execute
  this program and give it the path and filename of the item you tried
  opening. If you use an \#AV Protocol\# compatible application as your
  text file viewer, simply supply the full path of the application or desk
  accessory here.

Related topics:  \#Settings Menu\#
\end

\screen Mouse Pointer
Mouse Pointer
-------------

  The mouse pointer is controlled by the mouse and is used to control the
  NeoDesk desktop and its contents. With it you can select, \#open\#, and move
  different objects on the screen.

  Note:  Sometimes (very rarely) a desk accessory or application may cause
         the mouse to become confused and either disappear or leave
         "droppings" over the desktop. Most of the time, this is corrected
         automatically by NeoDesk. You can manually restore the mouse back
         to normal by holding down [CONTROL]-[LEFT SHIFT]-[ALTERNATE] and
         pressing the numeric keypad [PERIOD] key. A bell will sound to
         indicate that the mouse has been reset. This feature does not work
         with MultiTOS.

Related topics:  \#Using NeoDesk\#
\end

\screen Mouse Shape
Mouse Shape
-----------

  Similar to the built-in desktop, NeoDesk will use the standard arrow
  \#mouse pointer\# shape for most of its operations. However, when you perform
  file or folder move operations (similar to copy except that the files are
  physically moved from one location to another), NeoDesk will use the hand
  mouse shape. This visual clue will indicate what type of file operation
  is being performed. Also, whenever NeoDesk is busy doing something it
  will display the busy bee mouse pointer shape.

Related topics:  \#Using NeoDesk\#
\end

\screen Window Name
\screen Selecting a Different Path
\screen Moving Windows
Window Name Bar
---------------

  Once you \#open\# a window to a drive, the top of the window will display its
  current path and \#filter\#. This filter indicates which items are being
  displayed. You can change this filter (or add more) by using the
  Filter... command from the Sort window drop-down menu.

  You can choose any part of the path that is being displayed in a window's
  drag bar and double-click on it. That window's path will change to the
  part of the path you double-clicked on. This provides an easy way to jump
  to any part of the window's displayed path at any time.

  NeoDesk's \#windows\# can be moved just like any other GEM window. The
  window's top shaded bar (where the window name is also located) serves as
  a drag bar. You can select this area with the mouse (a window outline
  will appear) and move the window to a new screen position.

Related topics:  \#Windows\#
\end

\screen NeoDesk Menu
\screen About NeoDesk...
NeoDesk Drop-Down Menu
----------------------

  This drop-down menu is the first menu to the left of the desktop menu
  bar. The first entry, About NeoDesk..., displays the copyright,
  registered name, serial number, and other relevant information about your
  copy of NeoDesk 4. If you select the \#Using NeoDesk\#... button (or press
  the [HELP] key) you can access the top level of the built-in help system.

  Any menu entries below usually include your desk accessories (if you are
  using Geneva or MultiTOS this list will also include other loaded
  programs). If you choose to use the NeoDesk Control Panel, NeoDesk
  Printer Queue, or the NeoDesk Recoverable Trashcan, they will be listed
  in this menu along with any other installed desk accessories.

  If you are running NeoDesk inside a window, this menu will only contain
  the About NeoDesk... entry.

Related topics:  \#Desktop Drop-Down Menu\#
\end

\screen Create Folder...
\screen New folder
New Folder Name
---------------

  This entry will allow you to create a new folder in the path of the
  window it is selected on. \#Folders\# are not files, instead they hold files
  to separate them from other files. A folder may also contain other
  folders, down to eight levels deep.

  If "Open automatically" is selected, the new folder will be opened
  immediately after it is created.

Related topics:  \#File Window Menu\#
\end

\screen New Group...
New Group
---------

  Selecting this entry will allow you to create a new group in the current
  window directory. Once selected, the group creation dialog is displayed.

  Here you can enter the group's actual disk filename, the name it should
  have displayed in a window (this can include lowercase characters and
  spaces), and the group's description (available whenever you Show
  Information... on the group). See the section on \#Group\#s for more
  information.

  Once created, its icon will appear in the window directory and the
  group's window will be automatically opened.

Related topics:  \#File Window Menu\#  \#Opening Groups\#
\end

\screen Copying
\screen Moving
\screen Copying/Moving Items
\screen Copy/Move items
\screen New name for dest. file
Copying/Moving Files & \#Folders\#
------------------------------

  Copying or moving files and folders in NeoDesk is as simple as dragging
  any selected items from one window to another. You can also copy or move
  these items to a desktop drive icon, the desktop File \#Clipboard icon\#, or
  a desktop folder icon.

  When copying a file, the modification time and date of each file are
  preserved. NeoDesk does not allow a file to be copied to itself, as this
  might cause the file to become damaged (in older versions of TOS). Any
  file or folder operation can be stopped by pressing the [UNDO] key,
  holding down both [SHIFT] keys, or by using the [CONTROL]-[C] key
  combination.

  When moving a file or folder, NeoDesk physically moves the items to their
  new location. If the items are being moved within the same drive letter,
  NeoDesk moves the items by changing their path (leaving the files
  themselves intact). This operation takes about the same amount of time as
  renaming a file (technically they are the same thing). If a file is being
  moved to a different drive letter, NeoDesk will first copy the items and
  then \#delete\# the original versions.

  You can choose your default preference between copy and move operations
  in the \#File...\# dialog in the Settings desktop drop-down menu. With the
  default setting of Ask, NeoDesk will expect you to decide which type of
  operation you want at the Copy/Move items dialog.

  If you change your default to Copy or Move, you can also toggle the
  default operation by holding down the [CONTROL]-[LEFT SHIFT]-[ALTERNATE]
  keys while dragging the items you selected to their new location. NeoDesk
  changes the default mouse arrow shape to the shape of a hand to indicate
  when you are performing a move operation.

  When copying and moving files and folders, NeoDesk displays the Copy/Move
  items dialog. This dialog not only displays important information about
  the operation, but lets you change its settings. The upper left part of
  the dialog displays the destination path of the operation.

  The Copy and Move buttons let you choose between the two types of
  operations.

  When you copy any selected items, duplicates are made of them in the
  directory that you dragged the selected items to. When you move any
  selected items, the items are physically moved to the new location.

  When the "Confirm overwrites" button is selected, if you try copying or
  moving an item to where there already is an item with the same name, a
  filename conflict dialog will appear. At this point you can tell NeoDesk
  to overwrite the file, skip it, or cancel the entire operation. This
  option can be toggled in "real time" by holding down the [CONTROL] key.
  This is useful if you decided, halfway through an operation, that you no
  longer want to be alerted about overwrites.

  When the "Rename destination" button is selected, NeoDesk will prompt you
  for a new name for every item that is copied or moved.

  With the "Count items" button selected, NeoDesk will count how many total
  files and folders are involved in the current operation. This is useful
  to verify that NeoDesk is carrying out the operation you intended.
  However, this can also take additional time. Disabling this feature will
  speed up the entire operation (especially when a large number of items
  are involved). While counting the items, NeoDesk displays the name of the
  item being scanned. At the end of the count, the last item's name is left
  visible, so that you can at least see the name of one of the items before
  confirming the operation. If more than one item is being operated upon,
  the one visible name is followed by a "+".

  With the "In background" button selected, NeoDesk performs all operations
  in the background. This allows you to continue working on your machine
  while a file operation is taking place. If you are using a multitasking
  environment, you can even go on and continue using another program. On a
  singletasking system you are limited to the NeoDesk desktop and the
  loaded desk accessories.

  NeoDesk file and folder operations operate through a queue. If you
  attempt to copy, move, or delete additional items while a similar
  operation is already taking place, your request will be placed in queue.
  As soon as the current operation is completed, the Copy/Move items dialog
  will reappear ready for the next operation.

  The "Filter" button specifies if you want the operation to be \#filter\#ed
  through the Filtering dialog. The settings of that dialog are set using
  the Edit... button. Selecting this button opens us the filtering dialog,
  where you can set the type of filtering that you want applied to this
  operation.

  The dialog will remain visible (updating its information as the operation
  continues) until it is completed. While the operation is taking place,
  the "Ok!" button will change to read "Pause". This will let you
  temporarily pause the operation. The "Confirm overwrites", "Rename
  destination", "In background", "Copy", and "Move" buttons can all be
  selected while the operation is taking place.

  Taking any action which forces this dialog to close will cause the
  operation to terminate. This includes closing its window, closing the
  NeoDesk desktop window (if running as a desk accessory), or running a
  program in a non-multitasking system.

Related topics:  \#Using NeoDesk\#  \#Filtering Items\#  \#Name Conflicts\#
\end

\screen No Sort
No Sort
-------

  This entry will specify that the items should be listed in the natural
  order that they are stored on the disk (without any sorting at all). This
  is very useful for users who wish to find out what order their desk
  accessories or AUTO folder programs are in. In addition, you can use this
  command with \#Reorder Items\# to keep icons in any custom order you
  want. A check mark will appear to the left of this entry when it is
  selected.

Related topics:  \#Sort Menu\#
\end

\screen Disk information
\hidden On Drives
Show Info on Drives
-------------------

  When you show information for a drive icon, a very detailed report about
  the type of disk selected is displayed. This includes the total number of
  \#folders\#, files, hidden files, and hidden bytes on that disk. You are also 
  informed of the total bytes, bytes in files, and bytes free (still
  available) on the disk. Finally, the number of sides on the disk, its
  total sectors, the number of tracks per side, the number of sectors per
  track, and the number of bytes per sector are displayed.

  You will also have the option to create or change the NeoDesk \#Volume Name\#
  for that disk, providing that it is not write-protected. NeoDesk uses its
  own volume name format, which has several advantages including a 20
  character length and the ability to use lower and uppercase characters
  along with spaces. The NeoDesk Volume Name is stored in a hidden file
  called NEODESK.DAT.

  Since searching through the entire directory of a disk can take a very
  long time, there is a separate version, \#Quick Disk Info...\#, which does
  not count the number of files and folders. The operation can be aborted
  by holding both [SHIFT] keys.

Related topics:  \#File Desktop Menu\#  \#Show Information...\#
\end

\screen Folder information
\hidden On Folders
Show Info on Folders
--------------------

  When you Show Information on a folder, a dialog will appear which
  displays the folder's path (at the upper left of the dialog), name, and
  the time and date on which it was created. Also reported is the total
  number of files and folders inside that folder. Finally, you are told how
  many hidden files are in that folder along with how many bytes they are
  using.

  If you are running NeoDesk on a machine with TOS 1.4 or above, you can
  also change the folder's name.

  On machines with TOS 1.0 or TOS 1.2, you are not allowed to rename a
  folder. However, there is another way to achieve the same effect. That is
  to move the folder within the same directory. When you get the name
  conflict dialog, give it a new name. A new folder will be created and all
  of the contents of the old folder will be moved to it. Finally, the old
  folder will be deleted. Even on a floppy disk, the entire operation only
  takes a couple of seconds.

Related topics:  \#File Desktop Menu\#  \#Show Information...\#
\end

\screen Show Info on Groups
\hidden On Groups
Show Info on Groups
-------------------

  When you show information on a group, a dialog will appear which displays
  the group's path (at the upper left of the dialog), its filename, its
  name as it will appear in a NeoDesk window, and the group's description.
  You can also easily edit these.

  In addition, the dialog displays how many items are in the group, how big
  the group file is, its creation date & time, and the date of its last
  modification.

  This same dialog is also used when you create a new group.

Related topics:  \#File Desktop Menu\#  \#Show Information...\#
                 \#New Group...\#       \#Opening Groups\#
                 \#Change Item...\#
\end

\screen Open
\screen Opening Groups
Open
----

  The Open entry in the File desktop drop-down menu will open any icon that
  is presently selected. This is the same as double-clicking on an icon.
  If no icon is selected, this command will be grayed out.

  This command can also be used to open several icons at the same time. For
  example, you can select more than one group at a time, and open them all
  at once. You can also open multiple files (for viewing) at once, though
  they will be shown one after the other.

  If you are running a multitasking environment, you can select more than
  one program icon and open them at the same time.

  When you open a drive icon, NeoDesk will open a window to that drive.
  This will display that drive's root directory and all of the items in it.
  You can also open any drive icon by holding down the [SHIFT] key while
  pressing that drive's letter twice.

  If you open a folder inside of a window, that window's path will change
  to show the contents of the folder. If you open a desktop folder, a new
  window will open and display its contents. A letter can be installed for
  a desktop folder with the \#Install Desktop Icon...\# command in the Options
  desktop drop-down menu. A desktop folder can then be opened by pressing
  its icon letter twice.

  When you open a group icon, a separate window to that group will be
  opened. The size and location of that window is dependent to the group
  (since that information is stored within the group itself). This window
  will display the group contents. Similarly to desktop \#folders\#, you can
  install a letter on a desktop group for easy keyboard access.

  When you open a program icon, it is loaded and run. Once the program
  exits, NeoDesk will resume operation. If you are running a multitasking
  environment, NeoDesk will continue operating right after loading the
  program.

  If you have NeoDesk configured to automatically unload itself from memory
  whenever you run a program, NeoDesk will remove itself from memory before
  running the program. It will then wait until it regains control, and then
  load itself back into memory.

  Under most circumstances, in a multitasking environment, you will not
  want to unload NeoDesk. Doing so would result in NeoDesk unloading itself
  and immediately reloading itself (as it gets control back instantly after
  running a program). However, there are some circumstances where you
  might. If you have a big program that runs in single-tasking mode, you
  can tell NeoDesk to unload itself just for that program (either through a
  NeoDesk Program Information file or through the \#Install Application\#...
  command).  NeoDesk will remove itself from memory before running that
  program, and load itself back in when finished.

Related topics:  \#File Desktop Menu\#            \#Parameters for Execution\#
                 \#Passing Icons as Parameters\#  \#Active Icons\#
                 \#Installed Applications\#       \#Batch Files\#
\end

\screen Options Desktop Menu
Options Desktop Menu
--------------------

  The Options desktop drop-down menu controls many of the general options
  in NeoDesk. It contains these options:

    \#Install Desktop Icon...\#
    \#Install Application...\#
    \#Program Info File...\#
    \#Begin Macro\#  or  \#End Macro\#
    \#Execute/Change Macro...\#
    \#Save Configuration\#
    \#Load Configuration\#
    \#Edit Icons...\#
    \#Snap Icons to Grid...\#
    \#Set Video Mode...\#

Related topics:  \#Desktop Drop-Down Menu\#
\end

\screen Options Window Menu
Options Window Menu
-------------------

  The Options window drop-down menu controls many of the general options
  specifically related to files in the current window.  It contains these
  options:

    \#Install Application...\#
    \#Program Info File...\#
    \#Load Configuration\#

  A \#Group\# window menu also has these two entries:

    \#Snap Icons to Grid...\#
    \#Change Item...\#

Related topics:  \#Window Drop-Down Menus\#
\end

\screen Parameters for Execution
Parameters for Execution
------------------------

  When you run a TOS Takes Parameters ".TTP" or a GEM Takes Parameters
  ".GTP" program, NeoDesk automatically presents you with a Parameters for
  execution dialog.

  You can also call up this dialog for any other kind of program by holding
  down the [LEFT SHIFT] and [ALTERNATE] keys while \#open\#ing the program.
  Here you are able to enter and edit any parameters for that program,
  before it runs. This is also where you would control the file redirection
  for TOS and TTP programs. Since this is a toggle, if opening a program
  was already going to open a Parameters for execution dialog (usually
  programs ending in TTP, NTP, BTP, or GTP) then the opposite effect will
  occur and the dialog will not appear.

  When the Parameters for execution dialog is displayed, you can enter
  within it any parameters that you want passed on to the program before
  running it. Normally up to 126 characters can be entered, consisting of
  both upper and lowercase characters. If you select the Extended argument
  passing button (enabling the "xarg" method of parameter passing, which is
  supported by many programs), NeoDesk will pass an unlimited number of
  parameters.

  Selecting the Append... button will call up an Item Selector where you
  can choose any filename to add to the Parameters for execution dialog.
  The filename will be added with the shortest path possible and placed
  after any text already in the dialog.

Related topics:  \#Open\#                         \#Active Icons\#
                 \#Installed Applications\#       \#Batch Files\#
                 \#Input/Output Redirection\#
                 \#Passing Icons as Parameters\#
\end

\screen Passing Icons as Parameters
Passing Icons as Parameters
---------------------------

  Any file (or group of files) can be passed as parameters to any program
  by selecting those files and then, while holding down the [SHIFT] key,
  double-clicking on the program icon. The program will run with the
  selected files as parameters (the same method as used for \#Active Icons\#).

  Whenever NeoDesk passes a filename as a parameter to a program, it also
  passes the shortest possible path to that filename. This way the program
  being run knows where to find that file.

  This feature is based on the \#Install Application\#... feature. Therefore,
  it requires that the program you try using this feature with be designed
  to work as an installed application. The easiest way to find out is to
  test your favorite programs. Some will only accept one data file at a
  time (like 1st Word Plus) while other support up to four files at once
  (like Tempus). Other programs, like PageStream, Calamus, and WordPerfect
  also support this feature.

  When passing several filenames to a program which does not support
  extended argument passing ("ARGV"), the list of files is cut off at 126
  characters.

Related topics:  \#Open\#  \#Parameters for Execution\#
\end

\screen Help Topic Menu
\screen Previous
Help Topic Menu
---------------

  The Topic drop-down menu provides the functions necessary to navigate the
  help text.

  \x3 Find - This option lets you search for any specific topic in the help
    file (it supports the same \#wildcard characters\# used in the \#Filter\#
    \#dialogs\#). If the Match upper/lowercase differences option is set, then
    the case of the string you type must match exactly. If the topic is not
    found in the current help file, and the Search alternate help file path
    option is selected, then all files in the path set by the Set Help File
    Path option in the File menu will be searched until a match is found.

  \x3 Index - This option displays an alphabetical list of all sections of
    the help text.

  \x3 Previous - This option returns you to the previous help topic you were
    viewing. This can also be done by pressing the [UNDO] key.

  \x3 Using Viewer - This option will provide you with help on the Help
    Viewer itself.

Related topics:  \#Help Viewer Menus\#
\end

\screen Print Desktop
Print Desktop
-------------

  The Print Desktop entry will print a copy of the screen to a dot matrix
  printer. Everything on the screen is printed. This is not actually a
  NeoDesk feature, but one that is built into the operating system itself
  and therefore available inside any program. The width of the printer is
  set in the NeoDesk Printer Queue's printer settings. The Atari usually
  assumes that is printing to an "Epson FX" compatible printer, though some
  commercial and public domain programs will change the type of printer the
  screen dump can print to. This command can also be invoked from inside
  any program by pressing the [ALTERNATE]-[HELP] keys.

Related topics:  \#File Desktop Menu\#
\end

\screen Printer Icon
Printer Icon
------------

  The printer icon works either in conjunction with NeoDesk's Printer Queue
  desk accessory or all by itself. Whenever you wish to print a file, drag
  it to this icon. Multiple files can be selected and printed at once.

  If the Printer Queue desk accessory is loaded, the filenames are placed
  on the printer queue, which will then print them in the background (one
  after another). If the Printer Queue desk accessory is not loaded, you
  will have to wait until the last file has been printed.

  If either the Printer Queue or the NeoDesk Print Spooler (SPOOLxxx) are
  being used, you can hold down both [Shift] keys on the keyboard to
  remove any data that is in the spooler, and thereby stop printing.

  Note:  This method of printing sends the complete file contents directly
         to the printer. Therefore, plain text files (ASCII) are the ones
         best suited for this type of printing. Word processing and other
         files that belong to a specific program still must be printed
         through that program.

Related topics:  \#Icons\#
\end

\screen Program Information File...
\screen NeoDesk Program Information file
\hidden Program Info File...
NeoDesk Program Information File
--------------------------------

  A NeoDesk Program Information file (.NPI) is used to run a program with a
  unique set of settings. While similar to the \#Install Application\#...
  feature, NPI files are substantially more powerful.

  To create an NPI file, you first select this entry (a window must be open
  to a valid path). Using the "Program..." button, you can select the path
  and filename that this NPI references. Pressing the "Save" button will
  let you save the NPI file (you should give it a filename with an ".NPI"
  file ending). An NPI file can be placed in any directory and used to run
  the program it represents. For example, you could have a large program on
  drive D, but place an NPI for file it on drive C, therefore allowing the
  program to be run from an icon on drive C.

  By selecting an existing NPI file and then selecting this entry, you can
  change its settings.

  The "Name in window" entry allows you to specify a different name to be
  used in NeoDesk (instead of its actual filename). This does not change
  the filename, but makes the NPI easier to read. This name can consist of
  any 12 characters, including lowercase and the space character.

  The popup menu in the NPI dialog moves you between several groups of
  settings which will affect how the program gets run. All of these
  settings will override the way in which NeoDesk would otherwise run the
  program:

  \x3 The Main section controls how the program is executed, in a manner
    similar to \#Install Application\#.

  \x3 The Parameters section can be used to alter the way file names get
    passed to the program. Any text entered here is always sent, as-is. You
    can also use a $ followed by a number to indicate a particular passed
    filename, or $$ to indicate all passed file names. For instance, you
    could use the string "copy $1 $2" to indicate to a CLI that the first
    file should be copied to the second one. The "Extended argument
    passing" option allows more parameters to be passed than the standard
    limit of 126 characters. A program must specifically support this
    method.

  \x3 The Miscellaneous section gives you control over the hardware options
    that are enabled when the program gets run. Some programs do not work
    correctly when the CPU is at a certain rate or when the NeoDesk Control
    Panel's corner clock is on, so this allows these things to be changed.
    Not all of these options have an effect on all computers. The
    "Single-tasking" option only works when using Geneva 004 (or newer) or
    MagiC 2.0.

  \x3 The Environment section allows you to specify a group of up to ten
    \#environmental variables\# specifically for this instance of the
    program.

  Note that if you use NeoDesk 4 with a multitasking environment, like
  Geneva, then the Miscellaneous settings will not seem to work correctly.
  This is because the settings are changed back to their normal state as
  soon as NeoDesk regains control which, in a multitasking system, is
  immediate. With Geneva, you can solve this by setting the program itself
  to be single-tasking with Geneva's Task Manager.

Related topics:  \#Options Desktop Menu\#
\end

\screen Quick Disk Info...
Quick Disk Info
---------------

  This feature is identical to the \#Show Information...\# command, except that
  it only applies to disk drives and it will not include any file or folder
  related information. By excluding this information (which is the slowest
  to calculate) you can get a quick display of the type of disk and the
  free space on it, without having to wait for a complete file and folder
  count.

  If you choose this option when no \#icons\# are selected, and there is a
  window open, it will show information on the drive the window is
  currently showing.

Related topics:  \#File Desktop Menu\#  \#Show Information...\#
\end

\screen Quit NeoDesk
Quit NeoDesk
------------

  The Quit NeoDesk entry will allow you to quit NeoDesk. Before actually
  quitting, an alert box will appear asking you for confirmation. This
  alert box can be turned off from inside the \#File...\# dialog under the
  Settings desktop drop-down menu.

  Note:  You should make sure that you have saved your desktop
         configuration before quitting, or you might lose any changes made
         to the desktop.

Related topics:  \#File Desktop Menu\#
\end

\screen Reload .INF
Reload .INF
-----------

  Selecting this entry forces NeoDesk to reload its default preferences,
  just as if it had just finished loading. This is a very easy way to
  restore your desktop (including the screen colors) to the saved desktop
  preferences.

Related topics:  \#Settings Menu\#
\end

\screen Renaming Items
\screen Rename file/folder
Renaming Files & Folders
------------------------

  There are two different ways to rename files and folders. The easiest way
  is to select the items you want to rename and then select Show
  Information... from the File drop-down menu. There you can edit the
  item's name. You can rename multiple items by first selecting them all,
  and then selecting this command. The \#Show Information...\# dialog for each
  item will appear sequentially, one after the other.

  If you are using a machine with TOS 1.0 and TOS 1.2 you cannot rename
  folders. Instead you can cheat by \#moving\# the folder to the same directory
  it is currently in. When the Name Conflict dialog appears, enter the new
  name you want to give your folder.  NeoDesk will then move all of the
  folder contents to the new folder (and remove the original one).

  You can also rename a file or folder by holding down the [CONTROL] key
  while clicking once on its icon. A small dialog will appear which will
  let you rename the item.  Multiple items can be renamed by first
  selecting them, then holding down the [SHIFT] and [CONTROL] keys while
  clicking once on one of the selected items. A small dialog will appear
  for each selected item.

Related topics:  \#Using NeoDesk\#  \#Name Conflicts\#
\end

\screen Reorder Items
Reorder Items
-------------

  This entry places the window into a special reorder mode. All other
  NeoDesk \#windows\# and drop-down menu options are disabled. The items in the
  window (including any hidden and system files) will be shown in their
  natural (no sort) order. The window's name will change to indicate that
  this mode is now active.

  Using the \#mouse pointer\#, you can select a group of items and drag them to
  another icon at a new location within the window. This changes the order
  in which the item is stored on the disk. Dragging an item to an icon
  towards the beginning of the directory will place it right before any
  item you drop it on. Dragging an item towards the end of the directory
  will place that item right after the item you drop it on. You must drop
  an item on top of another item for this feature to work. Dropping an item
  in between other items or in a blank part of the window will be ignored.
  Some operations, like renaming a file, will change the window's order
  back to what it was before you started reordering it.

  Once you have finished reordering the directory, selecting Reorder Items
  a second time will allow you to either cancel your reordering or
  continue. At this point the order you selected will be implemented and
  everything will return back to normal.

Related topics:  \#Sort Menu\#
\end

\screen Rubberband Box
Rubberband Box
--------------

  The mouse rubberband box is used to select multiple \#icons\# or filenames.
  It appears when you hold down the left mouse button (make sure that the
  \#mouse pointer\# is not over an icon or window control) and then move the
  mouse in any direction.

  While the older built-in desktop's rubberband box can only be drawn
  towards the lower right of the screen, NeoDesk's rubberband box can be
  drawn in any direction.

Related topics:  \#Using NeoDesk\#  \#Selecting Items\#
\end

\screen Running Programs
Running Programs
----------------

  You can run programs from NeoDesk by double-clicking on their icon or by
  selecting them and then choosing the \#Open\# command in the File drop-down
  menu. The program will then load and run.

  If you are running a multitasking environment, NeoDesk will continue to
  run, ready to accept additional commands. Otherwise, once you exit the
  program, NeoDesk will resume operation.

  If you are running a multitasking environment and you select more than
  open program at once (either by dragging an outline around them or by
  holding down the [SHIFT] key while clicking on them) and then you select
  the Open command from the File desktop drop-down menu, the programs will
  be launched one right after the other, literally allowing you to run
  several programs at the same time.

  Otherwise, without a multitasking environment, only the first program
  selected will run.

Related topics:  \#Using NeoDesk\#
\end

\hidden Save Configuration...
\screen Save configuration
Save Configuration
------------------

  The Save Configuration... entry will let you save your desktop
  information file ".INF", your installed macros ".MAC", and your desktop
  notes ".NOT". These files are saved to the same folder that \#NEODESK\#.EXE
  was loaded from.

  By selecting the buttons to the left of the filenames, you can select
  which files should be saved. You can also rename the files as they are
  being saved.

  Remember that the default filenames for the configuration files that are
  automatically loaded when NeoDesk first loads are set in the \#INF Files...\#
  dialog in the Settings desktop drop-down menu.

  The Desktop information file ".INF" contains all your settings and
  preferences in the NeoDesk desktop. Everything you change is saved in
  this file. This way, when you run NeoDesk again, all your settings will
  be restored.

  A Macro file contains all your currently installed macros.

  A Notes file contains a copy of your desktop notes, their location, font,
  color, and size.

Related topics:  \#Options Desktop Menu\#  \#Desktop Notes\#
                 \#Macro assignments\#     \#Reload .INF\#
                 \#Load Configuration\#
\end

\screen Save Group
Save Group
----------

  The Save Group entry allows you to save all the information of a group
  to its file.

  Normally this happens automatically when you close a group. But if all
  you changed were some of the more minor settings, you may need to use
  this command to make sure that all changes are saved.

Related topics:  \#File Window Menu\#
\end

\screen Scrolling
Scrolling
---------

  You can scroll the window contents one icon row at a time by clicking on
  the scroll arrows of the window (there's one pointing up and one pointing
  down). The [UP ARROW] and [DOWN ARROW] keys can also be used instead of
  clicking on the scroll arrows.

  You can scroll the window one "page" at a time by clicking on the shaded
  area beneath or below the scroll bar, a "page" being the total portion of
  the path being shown within the window boundaries. Therefore, if a window
  can only show one third of all the items in a path, a "page" is equal to
  one third of all the items in that path. If all the items in a path fit
  within a window boundary, no shaded areas are displayed in the scroll
  bar. You can also scroll one "page" at a time by pressing the [SHIFT]-[UP
  ARROW] and [SHIFT]-[DOWN ARROW] key combinations.

  You can also scroll each window by moving its scroll bar to a new
  location within its column. NeoDesk supports real-time scroll bars. This
  means that the window contents will be updated (and will scroll) while
  you are moving the scroll bar with the mouse. This feature can be turned
  off from the \#Windows\#... dialog in the Settings desktop drop-down menu.

  If you hold down the mouse while pressing the scroll arrows or shaded
  scroll areas (or if you hold down the keyboard arrow keys) the window
  will start scrolling repeatedly. This makes it easier to scroll to any
  part of the window without having to click the mouse many times.

  Pressing the [CLR/HOME] key will bring the window back to the top of the
  current directory. Holding down the [SHIFT] key while pressing the
  [CLR/HOME] key will scroll the window to the bottom of the current
  directory.

Related topics:  \#Windows\#  \#Window Display Area\#  \#Keyboard Control\#
\end

\screen Scrolling With the Keyboard
Scrolling With the Keyboard
---------------------------

  You can scroll the contents of a window by using the [UP ARROW] and [DOWN
  ARROW] keys. This is the same effect as clicking on the window's scroll
  arrows.

  You can also scroll a window by a complete "page" (the area that fits
  within the window boundaries) by holding down the [SHIFT] key while
  pressing the [UP ARROW] and [DOWN ARROW] keys.  Pressing the [CLR/HOME]
  key will bring the window back to its topmost display location.

  Holding down the [SHIFT] key while pressing the [CLR/HOME] key will
  scroll the window to the bottom (where the last icons are shown).

Related topics:  \#Windows\#  \#Window Display Area\#  \#Keyboard Control\#
\end

\screen Searching Files
\screen Search...
Searching Files
---------------

  To search for a file you first have to decide where you want to search.
  You can select any single or series of \#drive icons\# (including the File
  \#Clipboard icon\#). You can also search for a file in any open window.  The
  largest and slowest searches are those that involve complete drives
  (which you perform by first selecting the icons for the drives you want
  to search). Smaller and faster searches are performed by first opening a
  window to the folder where the file you are looking for is most likely
  stored.

  If the Search... entry is chosen when there is a window open, it it
  operates on that window. One or more drives can also be searched by first
  selecting the icon for the drives and then choosing this option.

  A standard \#filter\#ing dialog will appear. Here you can enter the \#templates\#
  for any files you are looking for. You can also specify the size, date,
  time, and attributes for the files you are searching for.

  If the "Create group" button is unselected, the search routines will stop
  at each matching file, display its name, folder it is in, and its drive
  letter. You then have the option to Show it, to Skip it (and go to the
  next match), or to Cancel the entire operation. If you choose to show it,
  a window will open to the path where the item is, it will scroll down to
  there where it is, and it will be selected.

  If the "Create group" button is selected (which is the default) a special
  group will be created called "Search Results". This group will contain
  all the matching files found in the search. Once the search is complete,
  you have the option to save this group (by either attempting to close it
  or by selecting \#Save Group\# from the File window drop-down menu).

  A group only contains icons representing the original files. That means
  that the original files are still in their original location,
  undisturbed. You can see all the paths to all the files in the group by
  selecting \#Show as Text\# from the View window drop-down menu. You can, of
  course, use these group icons for file operations.

Related topics:  \#Using NeoDesk\#    \#Filtering Items\#   \#Templates\#
                 \#Selecting Items With the Keyboard\#
                 \#Searching for Items With the Keyboard\#  \#Selecting Items\#
\end

\screen Searching for Items With the Keyboard
Searching for Items With the Keyboard
-------------------------------------

  When you are in the new keyboard selection mode, you can search for an
  item by typing the first couple of characters in its name. For example,
  if you are looking for STENO.ACC, all you may need to type is "STE".

  The method used is very simple. NeoDesk will advance to the next item
  that matches the string you've entered every time you type a letter (in
  the keyboard selection mode). If you type "S", the first item that starts
  with the letter "S" becomes selected. If you follow with a "T", it will
  advance to the first item that starts with the combination "ST". Finally,
  when you type "E", it will advance to the first file that starts with the
  letters "STE". This makes it very easy to find a file, simply by typing
  its name.

  If a letter is not available, you will not be able to enter it. For
  example, if there are no files starting with "Z", then NeoDesk will
  ignore the "Z" when you type it.

  NeoDesk will print the string you are typing out on the left side of the
  \#window information line\#. You can also press the [BACKSPACE] key to erase
  letter you entered. Pressing the [ESC] key will update the window and
  exit from the keyboard selection mode. Pressing [UNDO] will just clear
  the string and remain in search mode.

Related topics:  \#Windows\#  \#Window Display Area\#  \#Keyboard Control\#
\end

\screen Select All Items
Select All Items
----------------

  This entry will select all the items in the window. This includes items
  which may not fit within the window boundaries, but are still in the
  displayed directory. The window's vertical scroll arrows will change (to
  a thicker arrow) to indicate if there are additional selected items off
  the top or bottom of the window's display area. Selecting this entry a
  second time will de-select all the selected items.

Related topics:  \#File Window Menu\#  \#Select All Items Gadget\#
\end

\screen Selecting Items
Selecting Files & Folders
-------------------------

  With the mouse, you can select files and folders by clicking on them with
  the left mouse button. If you want to select more than one, keep the
  [SHIFT] key pressed while clicking on additional items.

  Note:  NeoDesk lets you keep selected items that are beyond the window
         limits. If you scroll the window after selecting an item, it will
         remain selected even if it scrolls out of the window's visible
         area. This will be indicated by a thicker up arrow or down arrow
         in the window's scroll bar, indicating in which direction there
         are selected items.

  You can also create a \#rubberband box\# and use it to select a series of
  items. If you keep the [SHIFT] key pressed, you can create additional
  rubberband boxes to select additional series of items.

  If you just want to select every file in a window, use the Select All
  Items command from the File window drop-down menu, or the Select All
  Items button at the lower left corner of the window (it's the third
  button from the left).

  You can also select a larger series of icons by first \#filter\#ing them with
  the use of the Filter... dialog under the View window drop-down menu.
  Here you can filter any series of files and folders in the directory
  currently being shown. If you want the filter to also apply to folders,
  you need to de-select the All folders button. Once you've filtered out
  the items you want to select, you can then use the \#Select All Items\#
  command under the File window drop-down menu.

Related topics:  \#Using NeoDesk\#  \#Selecting Items With the Keyboard\#
                 \#Searching for Items With the Keyboard\#
\end

\screen Selecting Items With the Keyboard
Selecting Items With the Keyboard
---------------------------------

  Pressing the [TAB] key in NeoDesk places you in the keyboard selection
  mode. The item at the upper left of the window will become selected. Like
  a cursor, you can move around the contents of the window by using the
  keyboard arrow keys.

  Holding down the [SHIFT] key while using the arrow keys will let you go
  to the leftmost item on a row ([LEFT ARROW]), rightmost item on a row
  ([RIGHT ARROW]), down by a "page" ([DOWN ARROW]), and up by a "page" ([UP
  ARROW]). Pressing the [CLR/HOME] key will select the first item in the
  current directory. Holding down the [SHIFT] key while pressing the
  [CLR/HOME] key will select the last item in the current directory.

  You can perform any operation on the selected item just as if you had
  selected it with the mouse. You can also \#Open\# it by pressing the [RETURN]
  key. Other key combinations regularly used when opening items with the
  mouse also apply (pressing the [RETURN] takes place of a double-click).
  For example, [CONTROL]-[RETURN] to rename an item, [ALTERNATE]-[RETURN]
  to force viewing a file, and [ALTERNATE]-[LEFT SHIFT]-[RETURN] to open a
  program with the \#Parameters for execution\# dialog toggle.

  If you are within a folder, pressing the [ALTERNATE]-[ESC] key
  combination will close that folder and bring you up a directory (pressing
  this key combination at the root directory will close the window and exit
  the keyboards selection mode). Pressing the [TAB] key again will exit
  you out of this mode.

Related topics:  \#Windows\#  \#Window Display Area\#  \#Keyboard Control\#
\end

\screen Selecting Items With the Mouse
Selecting Items With the Mouse
------------------------------

  You can select any item in a window by clicking on it once with the
  mouse. The icon will then become "selected" and its filename will become
  reversed, meaning that you can now carry specific operations on it (like
  \#Open\#, \#Delete\#, etc.). Since NeoDesk supports a separate selected icon, the
  icon itself may change to reflect its new status.

  Whenever you select a new item, any previously selected items will be
  de-selected. However, if you hold down the [SHIFT] key while selecting
  new items, the previously selected items will remain selected. This makes
  it very easy to select several different items.

  You can also select multiple items by create a \#rubberband box\#. This is
  done by pointing the mouse at a blank part of the window and holding the
  left mouse button. As you move the mouse (while keeping the left mouse
  button pressed) a rubberband box will appear. When you release this box,
  any icons within its perimeter will become selected. The rubberband box
  can also be created in any direction, making it easy to select any range
  of icons.

  You can also hold down the [SHIFT] key to select multiple items when
  using the rubberband box. That way you can use several different
  rubberband boxes to select any series of items.

  NeoDesk also makes it very easy to select items beyond the window limits.
  This is done by continuing to hold the [SHIFT] key down while \#scrolling\#
  the window to a new location and selecting another item (or series of
  items).

  NeoDesk will indicate if you have any items selected off the top or
  bottom of the window by displaying a thicker and larger scroll arrow in
  the appropriate direction.

  You can also select all the items in any directory by selecting the
  Select All command from the File window drop-down menu. The selected
  icons will include any which are not visible in the current window
  (indicated by the thicker and larger scroll arrow in the appropriate
  direction).

Related topics:  \#Windows\#  \#Window Display Area\#
\end

\screen Settings Menu
Settings Drop-Down Menu
-----------------------

  The Settings desktop drop-down menu is where most of the desktop's
  settings can be modified. It contains these options:

    \#Memory...\#
    \#File...\#
    \#Desktop...\#
    \#Windows...\#
    \#Dialogs...\#
    \#Paths...\#
    \#Extensions...\#
    \#INF Files...\#
    \#Accessories...\#
    \#Environment...\#
    \#Miscellaneous...\#
    \#Reload .INF\#

Related topics:  \#Desktop Drop-Down Menu\#
\end

\screen Show as Icons
Show as Icons
-------------

  The Show as Icons entry will change the selected window to display all of
  its contents as icons. A check mark will appear to the left of this entry
  if it is selected. Items shown as icons are listed from left to right,
  top to bottom.

  The same function can be achieved by clicking on the
  \#Text & Icon Display Toggle\# window gadget.

Related topics:  \#View Menu\#
\end

\screen Show As Text
Show as Text
------------

  The Show as Text entry will turn off the icon display and force the
  window to display its contents as text. A check mark will appear to the
  left of this entry if it is selected. The items shown as text may be
  \#open\#ed, moved, or \#delete\#d just like when they are displayed as \#icons\#.

  The  character will be placed to the left of each folder, to
  differentiate them from regular files.

  When used in a \#Group\# file window, the Show as Text entry will also
  display the type of item an icon represents and its original path and
  filename.

  This function can be achieved by clicking on the
  \#Text & Icon Display Toggle\# window gadget.

Related topics:  \#View Menu\#
\end

\screen Show Date
Show Date
---------

  This entry will show the date of an item whenever the \#Show as Text\#
  option is enabled. The date format is specified in the
  \#Miscellaneous...\# dialog in the Settings desktop drop-down menu. For a
  file, this is the last date that it was modified. For a folder, this is
  the date on which it was created. A check mark will appear to the left of
  this entry when selected.

Related topics:  \#View Menu\#
\end

\screen Show Information...
Show Information...
-------------------

  The Show Information... entry in the File drop-down menu will display
  extra information about the item selected. The type of information
  displayed will vary depending on the type of item you select:

    \#On Drives\#  \#On Folders\#  \#On Groups\#  \#On Files\#

  If you choose this option when no \#icons\# are selected, and there is a
  window open, it will show information on the drive the window is current
  showing.

Related topics:  \#File Desktop Menu\#
\end

\screen Show Size
Show Size
---------

  This entry will show the file size of an item whenever the \#Show as Text\#
  option is enabled. These file sizes are in bytes. Use the
  \#Show true file consumption\# option in the \#Miscellaneous preferences\#
  dialog to show the actual amount of space consumed by a file. Use the
  \#Number format\# to choose a separator to make large numbers more readable.

  \#Folders\# display a size of zero. To display a folder's actual size, you
  should use the \#Show Information...\# command.

  A check mark will appear to the left of this entry when selected.

Related topics:  \#View Menu\#
\end

\screen Show Time
Show Time
---------

  This entry will show the time of an item whenever the \#Show as Text\#
  option is enabled. For a file, this is the last time that it was
  modified. For a folder, this is the time when it was created. A check
  mark will appear to the left of this entry when selected.

Related topics:  \#View Menu\#
\end

\screen AV Protocol
\screen Using NeoDesk as the AV Server
Using NeoDesk as the AV Server
------------------------------

  When the "Use NeoDesk for AV Server" option in the \#Miscellaneous preferences\#
  dialog is active, NeoDesk can communicate with other programs and desk
  accessories that support the AV Protocol. This means that you can do any
  of the following things with another program that also supports these
  parts of the protocol:

  \x3 The program can give NeoDesk some information to be saved into the
    current NeoDesk INF file. You must use the Save Configuration menu item
    to actually save the data to the INF file, once NeoDesk has received
    it.

  \x3 If the program's icon is on the desktop and it is already in memory
    when you double-click on it, it will be opened instead of executed a
    second time. The item must have an executable extension or .ACC for
    this to work.
 
  \x3 If you drag some items to the program's icon on the desktop, and it is
    already in memory, the program will be told to open those items.

  \x3 You can drag items to a window belonging to the application. The
    application can then do things like copy or open the items.

  \x3 NeoDesk can be told to open a window to a particular path, or update
    all windows containing a certain directory.

  \x3 NeoDesk can be told to run a program.

  \x3 A program can ask what is on the screen at a particular place.

  \x3 NeoDesk can tell the other application what font is being used to
    \#Show as Text\# in directory windows.

  \x3 If the alternate text viewer is an AV client, then only one copy will
    be run as you open text file after text file. You must supply the full
    path of the application or desk accessory in the \#Paths...\# dialog.

  (Technical note: In this mode, NeoDesk intercepts any requests for the
  application named AVSERVER. It is not necessary to configure anything
  else in order for NeoDesk to act as the AV Server. However, if you do set
  the environmental variable AVSERVER=NEODESK in the GEM environment before
  NeoDesk runs, then when NeoDesk does run, it will force this option on.)

  Prevent Multiple Instances
  --------------------------

  The AV Protocol includes a mechanism for the server (NeoDesk) to know
  that a given application understands the AV Protocol. However, several
  very popular programs never inform NeoDesk of this fact. Consequently, it
  is impossible for NeoDesk to know that they understand the VA_START
  message. This message can be used by the server to tell a copy of the
  application already in memory to open a new file when an icon is dragged
  to the program's own icon.

  So NeoDesk includes an option in the "Miscellaneous" dialog, "Prevent
  Multiple Instances". When this item is checked, NeoDesk will always check
  to see if there is already an application with a given name in memory. If
  there is, it does not launch a new instance, it instead sends a VA_START
  message to the application already in memory. As a result, you can drag
  items to the icon of a program like the text editor Everest and it will
  open the document.

  There is a disadvantage to this method, though. Because NeoDesk does not
  know if the application definitely supports the message, it has no way of
  knowing if you really intend for a new instance of the application to be
  run. When this option is selected, you will not be able to run more than
  one copy of any application at the same time. As an example, STeno does
  not support this message, so if this option is chosen, NeoDesk will never
  run more than one copy of it.

Related topics:  \#Miscellaneous preferences\#
\end

\screen Miscellaneous...
\screen Miscellaneous preferences
\screen Show true file consumption
\screen Number format
Miscellaneous
-------------

  The Miscellaneous... entry brings up the Miscellaneous preferences
  dialog. Here you can set any additional settings which are not covered by
  the previous options. There are:

  \x3 Show true file consumption - This option indicates if NeoDesk should
    display the true file consumption in addition to the file size.
    Normally, the only number shown is the file size. However, due to the
    method used to store files on a disk, a file will consume a slightly
    different number. With this option turned on, the file's true
    consumption will be shown right after the regular file size in either
    parentheses (when the file size is shown at the top of a window) or
    separated by a slash (when using \#Show as Text\#).

  \x3 Check for executable bootsector - This option indicates if NeoDesk
    should check for executable bootsectors. If the disk that the window is
    displaying has an executable boot sector, a small "E" indicator will
    appear in the upper right hand corner of the area where the NeoDesk
    \#Volume Name\# is shown (at the bottom right of the window).

    This is very useful if you are looking for a computer virus. Most disk
    viruses are stored as \#executable boot sectors\#. While this does not
    indicate that you do have a computer virus, if you see it on a disk
    that should not have an executable boot sector you should probably
    investigate further. If you are using a hard drive, the boot partition
    (usually "C") will have an executable boot sector. This is normal.
    Also, many autobooting games will have one. This is normal and should
    not be confused with a virus.

  \x3 Check for bad filenames - With this option selected, NeoDesk will
    restrict the characters that can be entered into its filename dialogs
    to the standard Atari filename characters.

  \x3 Viewer... - This button calls up a sub-dialog which lets you control
    the way pictures are displayed when opened for viewing. See
    \#Picture viewer options\# for more information.

  \x3 Use NeoDesk for AV Server - This option allows NeoDesk to communicate
    with other applications and desk accessories which support the
    \#AV Protocol\#.

  \x3 Prevent Multiple Instances - Please refer to the \#AV Protocol\# section.

  \x3 Use Kobold for file operations: If the Kobold program is running at the
    same time as NeoDesk, a message will be sent to it when a file
    copy/move/delete operation is started. Disk copying is unaffected.
    Also, the "Filter" and "Rename Dest." options are ignored.

  \x3 Diskcopy formatting - This option indicates what type of formatting
    should be used when performing a "with format" \#disk copy\# operation.
    The Normal option forces the regular formatting routines to be used
    (without any speed enhancements). The Twister option forces that option
    to be used, which is the fastest possible option for 10 sector disks).
    The Fast 11 option forces that option to be used, which is the fastest
    option for 9 sector disks. The Auto option allows NeoDesk to choose the
    fastest option depending on the type of format it needs to do (9 or 10
    sectors).

  \x3 Number format - When the Number Separator is non-blank, that character
    is inserted into numbers in many dialogs (like Show Info), as well as
    the \#Show as Text\# display. In the USA, you will usually want to use a
    comma, to produce numbers like "123,456".

  \x3 Date format - This option lets you specify the date format to be used
    when displaying the current date. When NeoDesk first runs, it decides
    which one to use based on your computer's nationality. However, you can
    easily change that setting here. In addition, you can also change the
    date separator.

  \x3 Report program exit status numbers - Whenever a program terminates
    it returns to the system a status number, which indicates under what
    conditions it terminated. With this option you specify what NeoDesk
    should do with these numbers. A status number of 0 is returned if the
    program exited without problems, therefore this number is never shown.
    With the All option enabled, all non-zero status numbers will be shown.
    With the < 0 option, only status negative status numbers will be
    displayed. With the None option, no status numbers will ever be shown.
    A negative number indicates a system error (the negative number
    indicating the error). A positive number indicates an internal program
    error.

    Since the built-in desktop never reported positive numbers, some
    programs incorrectly return a positive status number even though they
    exited without problems. If you discover that you are running one of
    these programs, you might want to set this option to only report
    negative status numbers "< 0".

Related topics:  \#Settings Menu\#
\end

\hidden Snap Icons to Grid...
\screen Snap icons to grid
Snap Icons to Grid
------------------

  The Snap Icons to Grid... entry will snap the desktop icons to a
  pre-defined grid size. A dialog is presented which allows you to
  customize the grid size.

  The shaded boxes represent the largest possible size of an icon
  (including its filename). By clicking on the arrow buttons you are
  allowed to set the distance between the icons.

  The "Defaults" button will restore the grid to its original position. The
  "Prevent overlapping" option will prevent icons from being placed
  directly on top of other existing icons. The "Snap" button will cause the
  desktop icons to snap to the specified grid.

  Whenever a "Snap" is performed, NeoDesk cleans up the icons so that they
  are drawn from top-down, left to right. This is visually the most
  pleasing way for the icons to be drawn (especially on slower machines).
  If you wish to have your icons cleaned up, without performing a "Snap",
  use the "Clean Up" button.  The placement of the icons will not be
  affected.

  When Snap Icons to Grid... is used in a group file window it, brings up
  this same dialog.  However, this dialog will now have an additional
  option. The "Re-sort by name" option will force all the icons in the
  group to be re-sorted alphabetically, using the name shown in the group.

Related topics:  \#Options Desktop Menu\#
\end

\screen Sort Items by Date
Sort Items by Date
------------------

  This entry will specify that the items should be listed in chronological
  order, \#folders\# before files. The more recently modified (or created)
  items will appear first. A check mark will appear to the left of this
  entry when it is selected.

Related topics:  \#Sort Menu\#
\end

\screen Sort Items by Name
Sort Items by Name
------------------

  This entry will specify that the items should be listed in alphabetical
  order, \#folders\# before files. A check mark will appear to the left of this
  entry when it is selected.

Related topics:  \#Sort Menu\#
\end

\screen Sort Items by Size
Sort Items by Size
------------------

  This entry will specify that the items should be listed according to
  their size in bytes. \#Folders\# are listed first in alphabetical order (as
  they do not display a size). The largest items will be listed first. A
  check mark will appear to the left of this entry when it is selected.

Related topics:  \#Sort Menu\#
\end

\screen Sort items by Type
Sort items by Type
------------------

  This entry will specify that the items should be listed alphabetically by
  their extenders, \#folders\# before files. A check mark will appear to the
  left of this entry when it is selected.

Related topics:  \#Sort Menu\#
\end

\screen Sort Menu
Sort Menu
---------

  This menu contains options designed to control how items are sorted
  within the window:

    \#Sort Items by Name\#
    \#Sort Items by Date\#
    \#Sort Items by Size\#
    \#Sort items by Type\#
    \#No Sort\#
    \#Reorder Items\#

Related topics:  \#Window Drop-Down Menus\#
\end

\screen System Crashes
System Crashes
--------------

  The Atari, like any other computer, will crash occasionally. Normally,
  when this happens, it presents you with a series of cherry bombs which
  are supposed to signify what type of error has occurred.

  The NeoDesk NEOLOAD.PRG utility installs a special bomb trap that will
  intercept most crashes and print the number of bombs to the screen, along
  with the type of error that they represent. Sometimes a system crash will
  be so complete, that even the bomb trap will crash and this report will
  not be displayed.

  Optionally, the bomb trap will display important register information
  which might help a programmer solve the problem that caused the program
  to crash. This information can be sent to the printer, instead of the
  screen, by pressing "P".
\end

\screen Special Window Buttons
\screen Window Duplicate
\screen x2
\screen Text & Icon Display Toggle
\screen Select All Items Gadget
\screen Previous Folder
\screen Item Information
\screen Trashcan Gadget
Special Window Buttons
----------------------

  The lower left part of the window has three special buttons that provide
  a number of useful features. These buttons each represent a separate
  function. However, when you are dragging an icon, they will change to
  reflect three additional functions that are only relevant when dragging
  an icon.

  \x3 Window Duplicate

  The first button (on the left) is the window duplicate button "x2" which
  when selected will open a second window the same path as the window you
  clicked on.

  \x3 Text & Icon Display Toggle

  The second button (in the middle) is the Text & Icon display toggle.
  Selecting this button will toggle the window display from icon view to
  text view, and vice-versa. This is identical to selecting \#Show as Text\#
  and \#Show as Icons\# from the View window drop-down menu.

  \x3 \#Select All Items\#

  The third button (on the right) is the Select All Items button. When
  selected, all the items in the current window directory will be selected.
  This will include any items currently not visible (their selection will
  be indicated by thicker and bolder scroll arrows). This is identical to
  selecting the Select All Items command from the File window drop-down
  menu.

  \x3 Previous Folder

  When the window is showing a path inside a folder and you are dragging an
  icon, the first button (on the left) will change to show the Previous
  Folder button (it looks like a 90 degree curved arrow). Dropping the item
  you are dragging over this button will copy it (or move it if you are
  performing a move operation) to the previous directory. For example, an
  item that was located in the C:\\ACC\\OLD\\ directory will be placed into
  the C:\\ACC\\ directory (the previous directory). This is sometimes also
  known as the "parent" directory.

  \x3 Item Information

  When dragging an icon, the second button (in the middle) will change into
  the Item Information button. Dropping an item over this button will show
  additional information about that item. This is identical to selecting
  the \#Show Information...\# command from the File window drop-down menu.

  \x3 Trashcan

  When dragging an icon, the third button (on the left) will change to show
  a small trashcan. Dropping an item over this button will \#delete\# it. This
  is identical to dragging the item to the built-in desktop trashcan. This
  feature was primarily designed to offer an easy way to delete an item,
  even if your trashcan is covered by a window.

  Note:  If you are using the recoverable trashcan, using this method for
         deleting files will NOT let you recover them. This button works
         just like the built-in trashcan.

Related topics:  \#Windows\#
\end

\screen Trashcan Icon
Trashcan Icon
-------------

  The trashcan icon is used to \#delete\# files or disks. Any disk, file,
  folder, or group you drag over to it will be deleted. You can remove the
  trashcan icon from the desktop, or add additional ones, by selecting the
  \#Install Desktop Icon...\# entry under the Options desktop drop-down menu.

  There is also a separate Recoverable Trashcan. This is a special desk
  accessory that allows you to recover any files deleted through it, at a
  later date. With few exceptions, the recoverable trashcan works just like
  the built-in trashcan. Any items (folders, files, or groups) that you
  drag to it will be deleted from your windows. It works on hard disks and
  floppy disks. The big difference is that these deleted items will be
  available inside the recoverable trashcan. You can access the recoverable
  trashcan contents by double-clicking on its icon (which is physically
  different than the built-in trashcan).

  However, to use the recoverable trashcan, you must have installed it and
  be running it (it is a separate utility designed to add functionality to
  NeoDesk 4).

Related topics:  \#Icons\#
\end

\screen Using NeoDesk
\screen NeoDesk
Main NeoDesk Help
-----------------

  This is the main level of help for NeoDesk. From here, you can access
  other topics by either using the "Find..." menu option in the help
  viewer, or by double-clicking on an underlined topic, below:

    \#Loading for the First Time\#
    \#Online Help Viewer\#

  For the most part, NeoDesk works just like the built-in desktop. This
  upward compatibility will make it easier for you to learn how to use
  NeoDesk. It also means that you do not have to forget all that you have
  learned while using the built-in desktop. You can \#open\# windows, copy
  files, and run programs just like you did before.

  On the top of the screen is a standard GEM drop-down menu. Since NeoDesk
  has more than one set of drop-down menus, we call this one the
  \#desktop drop-down menu\#. Right under the desktop drop-down menu is
  the desktop area. It is where all the \#icons\# and \#windows\# are displayed.

Related topics:  \#Mouse Pointer\#          \#Rubberband Box\#   \#Mouse Shape\#
                 \#Dialogs\#                \#Desktop\#          \#Templates\#
                 \#Icons\#                  \#Desktop Icons\#    \#Windows\#
                 \#Running Programs\#       \#Viewing Files\#    \#Creating Folders\#
                 \#Creating Groups\#        \#Selecting Items\#  \#Searching Files\#
                 \#Name Conflicts\#         \#Deleting Items\#   \#Formatting Disks\#
                 \#Copying Disks\#          \#Erasing Disks\#    \#Renaming Items\#
                 \#Copying/Moving Items\#   \#Desktop Drop-Down Menu\#
                 \#Window Drop-Down Menus\#
\end

\screen Set Video Mode...
\screen Video mode
Video Mode
----------

  The Set Video Mode... entry lets you set which video mode (this can
  include a different screen resolution and different amount of displayed
  colors) you want to use. The actual number of choices in this dialog will
  depend on the kind of computer and display you are using.

  If you are using a non-standard third-party video display card, you may
  only be able to change resolution through that display adapter's own
  software. Check your video display card's manual for more information.

  On some systems, such as those using an Atari monochrome display, this
  option is not available, as there are no other video modes available.

  Once you set your desired video mode, NeoDesk will switch to it. The
  method used to switch depends on your GEM/AES version:

  \x3 AES versions below 4.0 - NeoDesk will set the new resolution in the
    built-in desktop's DESKTOP.INF or NEWDESK.INF (depending on your TOS
    version) and then reboot the system. The system will then re-load into
    the new resolution.

    If you are using a utility like XBoot or SuperBoot, you should be
    careful to not have that utility set to overwrite the built-in
    desktop's information file. If they do, the resolution change that
    NeoDesk requested will not occur.

  \x3 AES versions 4.0 and above (this includes Geneva and MultiTOS) -
    NeoDesk will ask the operating system to switch resolutions. All tasks
    are shut down, the system switches to the new resolution, and all desk
    accessories are automatically reloaded. This is faster than the method
    used in older versions of the AES.

Related topics:  \#Options Desktop Menu\#
\end

\screen View Menu
View Menu
---------

  The window's View drop-down menu contains commands that affect how the
  information is displayed within the window.

    \#Show as Icons\#
    \#Show as Text\#
      \#Large Text\# or \#Small Text\#
      \#1 Column\# or \#>= 1 Column\#
      \#Show Size\#
      \#Show Date\#
      \#Show Time\#
    \#Filter...\#
    \#Print Directory...\#
    \#Update Window\#
    \#Force Update\#

Related topics:  \#Window Drop-Down Menus\#
\end

\screen Viewing Files
Viewing Files
-------------

  You can view text files and picture files directly from the NeoDesk
  desktop by double-clicking on them or \#open\#ing them. The file will then be
  displayed using NeoDesk's internal file viewer. This file viewer supports
  both plain text (ASCII) documents and pictures in the more popular
  formats (IMG, NEO, PI1-3, PC1-3, TN1-3, TNY, BMP).

  If the file ending of a text file or picture file has been assigned an
  application (though the \#Install Application\#...  command) then opening
  that file will launch its assigned application. You can still view that
  file, by holding down the [ALTERNATE] key when opening the file. This
  even works for viewing program files.

  When a text file is shown to the screen, it is displayed without
  formatting just as it is stored on the disk. When all the screen lines
  are filled, a "<MORE>" prompt is shown (at the bottom of the screen).

  Pressing the [RETURN] key will display the next line, while pressing the
  [SPACEBAR] will display the next screen. Clicking on the left mouse
  button or pressing the [DOWN ARROW] key will also display the next
  screen. Pressing the [UP ARROW] or right mouse button will go back a
  screen.  You can stop viewing the text file and return to the desktop at
  any time by pressing the "Q" key, the [ESC] key, the [UNDO] key, the
  [CONTROL]-[C] keys, or by pressing both mouse buttons at once.

  NeoDesk also has a secondary text file display mode, to which you can
  toggle back and forth by pressing the [CONTROL]-[Z] keys. In this
  secondary mode, you view a file by pressing any key as a toggle, to pause
  or continue its display. Pressing [CONTROL]-[Z] again will return you to
  the normal file display mode. If you invoke this secondary file display
  mode, it will remain in effect until you toggle it back to the normal
  display mode.

  When viewing a picture file, NeoDesk will switch to any available system
  resolution to correctly show the picture. If the picture you attempt to
  view does not match the current number of screen colors, it will be
  converted to monochrome (though a process known as "dithering"). Pressing
  any key or pressing any of the mouse buttons while the picture is being
  viewed will exit back to the NeoDesk desktop.

  Some pictures (particularly IMG and BMP formats) are sometimes too large
  for the current screen resolution. In that case, only the top leftmost
  portion of the picture will be displayed.

  If you wish to use another file viewer (there are several very good ones
  in the public domain and in shareware), you can do so by entering its
  path and name into the \#Paths...\# dialog in the Settings desktop drop-down
  menu.

Related topics:  \#Using NeoDesk\#  \#Picture viewer options\#
\end

\screen Volume Name
Volume Name
-----------

  At the bottom of the window, where the horizontal window bar used to be
  on the built-in desktop, NeoDesk will display the NeoDesk Volume Name of
  the disk that the window is currently showing. The NeoDesk Volume Name
  can also be created and edited whenever you format a floppy disk or use
  the \#Show Information...\# command in the File desktop drop-down menu on a
  drive icon.

  Note:  The NeoDesk Volume Name is stored in a hidden file on the root
         directory of each disk (NEODESK.DAT). It allows you to give each
         disk a unique 20 character name. This name can include lowercase
         letters and spaces.

  You can create or edit a NeoDesk Volume Name by holding down the
  [CONTROL] key while clicking on the horizontal bar at the bottom of the
  window. An editing field will appear where you can edit the volume name.
  You can use upper and lowercase letters along with spaces. After editing,
  pressing the [RETURN] key will update the volume name to the disk.

  If there is no NeoDesk Volume Name on a disk, the window will display the
  Atari Disk Label of that disk (if there is one). The Atari Disk Label is
  the name assigned to the disk if you formatted it outside of the NeoDesk
  environment.

  Any attempt to edit the disk label will automatically convert it to a
  NeoDesk Volume Name. The advantages of this are several: a NeoDesk Volume
  Name is longer (20 characters), allows lowercase characters, and lets you
  use spaces.

Related topics:  \#Windows\#  \#Executable Boot Sectors\#
\end

\screen Templates
\screen Wildcard Characters
Templates & Wildcards
---------------------

  File templates are the most powerful feature of the \#filter\# dialog. To
  edit a template, click on its editable field. To activate it (so that it
  is used), select its button. You can have up to six active templates at
  once.

  Both the "*" and "?" wildcards are fully supported. The "*" character
  represents any number of characters, while the "?" character represents
  any single character. For example, a template of "*.ACC" would show all
  the files in a window that end a ".ACC" file ending. Templates of
  "NEO*.*" and "*.?C?" will show all files whose starting letters are "NEO"
  or who have three letter extensions of which the second letter is "C".

  When using file templates to filter a window's contents, you can
  de-select the All folders button to force the file templates to also be
  applied to the folders. The disadvantage is that it then becomes harder
  to navigate through the window, since the folders are not visible.

  You can also use the Free-form templates. These templates allow for more
  powerful shorthand when creating a file template.

  In addition to the regular "?" and "*" wildcard characters, free-form
  templates support the "{" and "}" to indicate the beginning and end of a
  list of choices, and the "[" and "]" characters to indicate a range of
  letters.

  For example, the free-form template of "*.{PI1,PI2,PI3}" would select all
  Degas pictures (which end in PI1, PI2, or PI3). The free-form template of
  "*.{PI[1-3]}" does the same thing (the [1-3] indicates a range that
  should be used to replace the last letter of the"PI" file ending. Ranges
  can also include letters, like [A-G]. Multiple free-form templates can be
  separated by a comma "PI[1-3],NEO*.*", allowing for multiple templates to
  be easily written together.

  In addition to these wildcard characters, NeoDesk also supports "!"
  wildcard character. This character can be used for inversion (not) of the
  next item in a string. If the next item is just a regular character, then
  that one letter will be inverted; if the next item is a "{ }" or "[ ]"
  bracketed expression, then the meaning of that expression will be
  inverted. For example:

  *.prg             Any filenames with a PRG file ending
  *.!abc            Any matching "*.?BC", excluding any matching "*.ABC"
  *.ab!c            Any matching "*.AB?", excluding any matching "*.ABC"
  *.!a!bc           Any matching "*.??C", excluding any matching "*.ABC"
  [a-d]*.*          Any starting with A, B, C, or D
  ![a-d]*.*         Any not starting with A, B, C, or D
  *.{prg,prx}       All ending in PRG or PRX
  *.!{prg,prx}      Any not ending in PRG or PRX

  Any number of "!" characters can be used within a wildcard. The effect is
  that the expressions are negatively added together. For example:

     !{stalker,neodesk?}.!{ac[cx],rsc}

  Matches anything but:

     STALKER.ACC         STALKER.ACX         STALKER.RSC
     NEODESK?.ACC        NEODESK?.ACX        NEODESK?.RSC

  However, NEODESK.ACC will match, since it does not have a character
  between the "K" and the period.

Related topics:  \#Searching Files\#  \#Filtering Items\#
\end

\screen Window Close Button
Window Close Button
-------------------

  The window close button, at the upper left corner of the window, works
  just like the built-in desktop's close button. The behavior of this
  button depends on the path being displayed in the window.

  If the window is displaying a path at least one folder deep, the window
  will go out of the current folder and back to the previous parent path.
  If the window is displaying the root directory of a disk when the button
  is selected, the window will be closed.

  You can also have the active NeoDesk window, if it is displaying a path
  at least one folder deep, go out of the current folder and back up to the
  previous path by pressing the [ALTERNATE]-[ESC] keys. Pressing the
  [CONTROL]-[CLR/HOME] keys will return the active NeoDesk window to the
  root directory of the drive it is displaying. If the topmost window is
  already displaying the root directory of the drive, [ALTERNATE]-[ESC]
  will close it. Holding down the [CONTROL] key while clicking on a NeoDesk
  window's close button will close it, regardless of its current path. The
  topmost NeoDesk window can also be closed by pressing the [CONTROL]-[W]
  keys.

Related topics:  \#Windows\#  \#Close Folder\#  \#Close Window\#
\end

\screen Window Cycle Button
Window Cycle Button
-------------------

  The window cycle button will cycle through your active \#windows\#. If you
  are not running Geneva, it will only cycle through the open NeoDesk
  windows. This makes it easier to arrange multiple windows and get to
  windows which might be hidden behind other windows.

Related topics:  \#Windows\#
\end

\screen Window Display Area
Window Display Area
-------------------

  The window display area (the area within the window boundaries) displays
  any \#folders\#, files, programs, and groups that are visible in the current
  path (as shown at the top of the window). The items displayed will also
  be limited by any \#filter\#s being imposed on the window.

  NeoDesk displays icons and files from left to right, top to bottom. The
  icons are automatically reformatted to the current window width (and get
  reformatted every time you move the window). This means that you only
  have to scroll the window vertically when looking for files.

Related topics:  \#Windows\#  \#Keyboard Control\#  \#Scrolling\#
                 \#Selecting Items With the Mouse\#
\end

\screen Window Drop-Down Menu
Window Drop-Down Menu
---------------------

  The window drop-down menu contains those commands that are most pertinent
  to the contents of the NeoDesk \#windows\#. Some commands (especially in the
  File menu but also in the Options menu) are duplicated from the desktop
  drop-down menus, which makes it easier to find them.

  This menu bar works just like a desktop menu bar, and can be accessed by
  clicking on it with the mouse. However, unlike the desktop menu bar, the
  menus will only open when they are clicked on with the mouse. This is
  intentional, as it prevents them from opening whenever the mouse passes
  over them.

  If you are not using Geneva, you can open the window drop-down menu of
  the topmost window by holding down the [ALTERNATE] key while pressing the
  [SPACEBAR]. This will allow you to use the window drop-down menu directly
  from the keyboard. If you are using Geneva, this sequence will first open
  the desktop drop-down menu (pressing it a second time opens the drop-down
  menu of the topmost window).

  Once the drop-down menu has been opened this way, you can move around
  them by using the keyboard arrow keys. Pressing the [RETURN] key will
  activate the currently selected entry. You will also see a small
  underline under a letter of each menu entry, allowing you to choose that
  entry by pressing its underlined letter (similar to how keyboard
  equivalents are handled in the NeoDesk dialog buttons).

  Pressing the [ESC] or [UNDO] key will close the window drop-down menu,
  when it has been opened this way.

Related topics:  \#Windows\#
\end

\screen Window Drop-Down Menus
Window Drop-Down Menus
----------------------

  Each file window in NeoDesk has its own set of drop-down menus. Here are
  all the commands that relate specifically to that window. Some are
  duplicated from the desktop's drop-down menus, for your ease of use.

Menus:  \#File Window Menu\#  \#View Menu\#  \#Sort Menu\#  \#Options Window Menu\#

Related topics:  \#Using NeoDesk\#
\end

\screen Window Information Line
Window Information Line
-----------------------

  Underneath the window's drag bar is the window's information line. This
  line displays important information about the window's contents. If there
  is not enough space within the information line to display its contents,
  left and right arrow buttons will appear that can be used to scroll its
  contents.

  You can also scroll the information line by holding down the [CONTROL]
  key while pressing the "(" or "/" keys on the numeric keypad to move left
  and right, respectively.

  Normally, the information line will show the total number of items in the
  path being displayed. It will also show the total size of the files in
  the current path. This size does not include any folder in the current
  path. To see a specific folder's size, you need to use the Show
  Information... entry in the File window drop-down menu.

  The size shown is the total size of the files in the current directory.
  It does not accurately display how much space those files are consuming
  on the drive (this number depending on how the drive was formatted).
  There is a setting in the \#Miscellaneous...\#  dialog in the Options desktop
  drop-down menu that will enable NeoDesk to display (in parentheses next
  to the total size) the exact amount of drive space being consumed.

  When a single file, program, or group is selected, the window information
  line will display the file's size in bytes along with its last
  modification time and date. If you've turned on the option to
  \#Show true file consumption\# in the Miscellaneaous... dialog in the
  Settings desktop drop-down menu, it will display this information next to
  the file size (within parentheses).

  It will also display that file's read/write status and its archive bit
  status. The read/write status tells you if a file can only be read or
  read and written to. An "R" is shown if the file is read-only and a "R/W"
  if the file is in read/write mode. A read-only file cannot be \#delete\#d or
  modified. You can change a file's read/write status by selecting it and
  using the \#Show Information...\# command in the File window drop-down menu.
  If the file's archive bit is turned on, an "A" is shown.

  When a single folder is selected, the window information line will
  display the folder's creation time and date.

  When more than one item is selected, the window information line will
  display the total number of items selected. It will also display the
  total size of all the selected files. The folder sizes are not
  calculated, since that would require reading the disk again. To calculate
  a folder's size, select its icon and then use the Show Information...
  entry in the File window drop-down menu.

  You can select several items at once before using the Show Information
  command. NeoDesk will select each item and display its information, one
  at a time, in succession.

Related topics:  \#Windows\#
\end

\screen Windows...
\screen Window preferences
Window Preferences
------------------

  The Window preferences dialog is where you can customize the display and
  behavior of the NeoDesk file \#windows\# (plus you can also choose the icon
  font for all \#icons\#, including desktop ones).

  The upper left portion of the dialog displays a sample of what the
  window's contents will look like with the current settings. To the right
  of that you can set the Pattern, Fill color, and Text color of the
  window. Selecting these buttons brings up a list of all available
  patterns and colors, respectively. You can also use the arrows to scroll
  through the available choices.

  The "Opaque text" button sets if the window text should have its own
  white background (making it easier to read on a pattern) or whether the
  pattern should show through the text. This only affects \#Show as Text\#
  mode, as icon names always have a white box around them.

  The "Fonts" selection lets you choose a different font for each size
  selection available in NeoDesk. The "Icon font" is used by all icons
  (including desktop ones). The "Small text font" is used whenever you have
  both the \#Show as Text\# and \#Small text\# options selected in the View
  window drop-down menu. The "Large text font" is used whenever you have
  both the \#Show as Text\# and \#Large text\# options selected in the View
  window drop-down menu.

  By selecting the "Name:" button you can choose the font you want to use;
  from a pop-up list of the available system, GDOS, or Speedo-GDOS fonts.
  The "Point size:" selection lets you choose any of the available point
  sizes for that font.

  The Icon font can be any system, GDOS, or SpeedoGDOS font.  However, the
  Small and Large fonts must be a non-proportional font. That means that
  each character must have the same width.  When you try selecting a font
  for one of these options, fonts that are proportional will be grayed out.

  If the "Real-time scroll bars" button is selected, then whenever you drag
  the vertical slider in the topmost NeoDesk window, the contents will be
  scrolled in "real time" as you drag the slider. Disabling this option
  returns the slider behavior to the way the built-in desktop works (where
  you must first release the slider before the window is updated).

  If you are not using Geneva, then the \#Gadgets\# button will allow you
  to modify the colors and keyboard equivalents for window gadgets.

Related topics:  \#Settings Menu\#
\end

\screen Window Resize Gadget
Window Resize Gadget
--------------------

  To the lower right corner of the window is the resize button. You can
  select this button with the mouse pointer, and while keeping the left
  mouse button pressed, resize the window to a new size. The window outline
  that appears lets you see what size the window will be resized to when
  you release the mouse button. Once the mouse button is released, the
  window will be resized to its new size.

  If, while dragging the window size outline, you move the mouse past the
  left or top border of the window, you can resize the window in those
  directions. In other words, if you drag the window size outline to the
  upper left corner of the screen, the window will be resized towards the
  upper left corner of the screen, leaving the lower right corner where it
  is.

Related topics:  \#Windows\#
\end

\screen Window Split Bar
Window Split Bar
----------------

  NeoDesk has the ability to display split \#windows\#. This can be used to
  view two different parts of the same window. To use this feature, select
  and drag down the thin horizontal bar that is located directly below the
  \#window information line\#. Wherever you place this bar is where the window
  will be split. Two vertical scroll bars will be displayed.

  Each part of the split window can be scrolled to its own individual
  location. Dragging the thin horizontal bar back to its original position
  restores the window back to its original state. If the window is too
  short to be divided, the bar cannot be dragged.

Related topics:  \#Windows\#
\end

\screen Windows
Windows
-------

  NeoDesk uses its own custom windows, which offer many advantages over the
  regular GEM windows. You can \#open\# a NeoDesk window to any drive, desktop
  folder, or desktop group icon by double-clicking on it.

  The NeoDesk windows do not use the "grow" or "shrink" boxes that the
  built-in desktop has, thus decreasing the amount of time needed to open
  or close a window.

Related topics:  \#Window Close Button\#      \#Window Name\#
                 \#Window Cycle Button\#      \#Window Full Size Button\#
                 \#Window Information Line\#  \#Window Drop-Down Menu\#
                 \#Window Split Bar\#         \#Window Display Area\#
                 \#Scrolling\#                \#Keyboard Control\#
                 \#Special Window Buttons\#   \#Volume Name\#
                 \#Window Resize Gadget\#     \#Independent Windows\#
                 \#Background Windows\#
                 \#Scrolling With the Keyboard\#
                 \#Selecting Items With the Mouse\#
                 \#Selecting Items With the Keyboard\#
                 \#Searching for Items With the Keyboard\#
\end

