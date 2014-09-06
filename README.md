DataViewer
==========

 Qt application example

 + About Git contents
	++ Folder "DataViewer" contains Qt project source code. In order to build from source code, download the source code and
	imported the project in Qt5 creater. Configure based on OS.
	++ Folder "executables" contains builds under linux-ubuntu32bit and win7-32bit 
	++ Folder "testcases" contains the data file from MineSense

 + To use the application
	++ Under "File" menu, there are common file operations, e.g., New, Open, Save, Save As and Exit.
	++ For the table view
		+++ Data could be edited when double click on it. "Energy" will be in data type "double" and "Counts" will be in 
		unsigned int.
		+++ Right click on the table, a menu for "insert" and "remove" will pop-up
		+++ When load data from file, or modify the data, the data will be sorted by "Energy" in ascending order
	++ For the graph view
		+++ The figure updates when data updates
		+++ Support "Zoom in", "Zoom out", "Move up", "Move down", "Move left" and "Move right"
			++++ Left click mouse on the graph, move mouse to draw a rect, release the mouse. The selected rect area will 
			be zoomed
			++++ To zoom out, use one of options below:
				+++++ Right click mouse
				+++++ Click "Zoom out" icon
				+++++ Press "-" key
			++++ To zoom in back, use one of options below:
				+++++ Click "Zoom in" icon
				+++++ Press "+" key
			++++ To move figure, press "up", "down", "left", "right" key, or use mouse scroll