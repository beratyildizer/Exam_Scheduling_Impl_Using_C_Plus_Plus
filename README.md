You can use makefile directly or any compiler supporting at least C++ 20 can be used. I give main code in termProject.cpp and you can use it just customizing input csv files. 
Please locate csv files at the same location with the project. If our code cannot find input files, it will not work properly. 
You just need to change input files to test our program without customizing header columns. I performed many tests and tried many input sets . 
Also, after compiling and running our code, you can check console screen to be sure that exam scheduling is generated or not. If generated, go to exam_schedule_program.csv to see examination scheduling times.

First of all, I would prefer to give a brief introduction about which methods we used to satisfy term project requirements before explaining our code. I implemented an undirected graph structure to show relationships between classes. 
If a class is taken by at least 2 different students or is given by the same professor, I add a relationship to these classes. 
To put it in another way, our classes are vertex in the graph and there is an undirected edge between two classes if a class is taken by at least 2 different students or is given by the same professor as said above.
I decided to use the adjacency matrix method to implement graph structure. After implementing graph structure and showing relationships between classes(nodes) via edges, I used Graph Coloring method. 
If two classes or vertex has no connection, we assigned same color these classes(vertex). It means that examination time for classes having the same color can be at the same time and we defined examination times following that. 
The last point is that I decided to use Welsh Powell Graph coloring algorithm. So, Welsh Powell algorithm is implemented in graphColouring.
As a last point, output is the final schedule, including assigned exam times for each class, and any blocked hour.
