#include "examScheduling.h"
#include <algorithm>
#include <iostream>

static string day_of_array[]{ "monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday" };

/*
   * Each room and its availability is stored in a matrix. First index specifies room, second index specifies days, and third index
   * specifies hour between 9 and 18.
   */
static vector<vector<vector<int>>> room_time_list(theNumberOfRooms, vector<vector<int>>(7, vector<int>(10)));

/*
* This tuple object is responsible for storing booking information. Each element of tuple is room, day index, start hour,
* start minute, finish hour, finish minute respectively.
*/
static vector < tuple<string, int, int, int, int, int>> bookHours{};

/*
*  This function is used to read available classes from ClassLists.csv.
*/

void readClassList(const std::string& filename, std::vector<classLists>& classes)
{
	ifstream file(filename);

if (!file.is_open()) {
    throw myException{};
}

std::string line;
std::getline(file, line);

while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string id, professorName, courseID, token;
    int studentID, examDuration;

    std::getline(iss, id, ',');
    std::getline(iss, professorName, ',');
    std::getline(iss, courseID, ',');
    std::getline(iss, token, ',');
    studentID = std::stoi(id);
    examDuration = std::stoi(token);

    classes.emplace_back(studentID, professorName, courseID, examDuration);
}

file.close();
}

/*
*  This function is used to read available classes from RoomLists.csv.
*/

void readRoomCapacities(const std::string& filename, std::vector<roomList>& rooms)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw myException{};
    }

    std::string line;
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string roomID, token;
        int capacity;

        std::getline(iss, roomID, ',');
        std::getline(iss, token, ',');
        capacity = std::stoi(token);

        rooms.emplace_back(roomID, capacity);
    }

    file.close();
}

/*
*  This function is used for taking available classes from vector. map container is used since it store
*  a pair where class id will be used as key and an integer will be used as value. Integer used value is used for
*  generating graph adjanceny matrix.
*
*/

map<string, int> readClassesfromVector(vector<classLists>& vec)
{
    map<string, int> mapClass;
    int count{};

    for (auto& i : vec) {
        if (mapClass.find(i.courseID) == mapClass.end()) {
            mapClass[i.courseID] = count++;
        }
    }

    return mapClass;
}

/*
*  This function is used for generating a graph using adjancency matrix method. The size of matrix is
*  taken from map container generated in readClassesfromVector. Also, adding edge is performed in this function
*  checking the whether a class is taken from same student.
*
*/

myGraph generateGraphForClasses(map<string, int> mymap, vector<classLists>& vec)
{
    myGraph temp = mymap.size();

    for (auto i = vec.begin(); i != vec.end(); ++i) {
        for (auto j = i + 1; j != vec.end(); ++j) {
            if (i->studentID == j->studentID || i->profName == j->profName) {
                temp.addAdge(mymap[i->courseID], mymap[j->courseID]);
            }

        }
    }

    return temp;

}

/*
*  This function is used by graphColouring to be sure that vertices do not have any connection. If there is
*  no connection, they can have same colour.
*/

bool checkNeighboor(vector<string>& vec, string& s, myGraph& graph, map<string, int>& mymap)
{
    for (auto i = vec.begin(); i != vec.end(); ++i) {
        if (graph.checkIsItNeighboor(mymap[*i], mymap[s]))
        {
            return true;
        }
    }
    return false;
}

/*
*  This functios is impelemented for graph colouring using Welsh Powell algorithm.
*  mymap : It stores classes and their ID's used in Adjancecy matrix as index.
*  graph : It is an object managing adjancecy graph process.
*/
map<int, colorList>  graphColouring(myGraph& graph, map<string, int>& mymap)
{
    using pairForClouring = pair<string, verticesDegree>;

    /*
    *   This vector is generated for storing class and their vertices degree located in Adjancecy matrix.
    */
    vector<pairForClouring> myvec;
    myvec.reserve(mymap.size());
    /*
    * This colorTable object will store class ID loceted in mymap and their colors.
    */
    map<int, colorList> colorTable;

    colorList color{ white };

    /*
    * This checkColor object will be used to color all the vertices which is not connected to the coloured vertex, with the same color.
    */
    vector<string> checkColor;

    for (auto i = mymap.begin(); i != mymap.end(); ++i) {
        myvec.emplace_back(pairForClouring{ i->first, graph.verticesDegree(i->second) });
    }

    /*
    *  This sort function is called to store classes according to their verticesDegree
    */
    sort(myvec.begin(), myvec.end(), [](pairForClouring& x, pairForClouring& y) { return x.second > y.second; });

    for (auto i = myvec.begin(); i != myvec.end(); ++i) {
        for (auto j = i; j != myvec.end(); ++j) {
            if (colorTable.find(mymap[j->first]) != colorTable.end()) 
              continue;

            if (i == j) {
                colorTable[mymap[i->first]] = color;
                checkColor.push_back(i->first);
            }
            else {
                if (!checkNeighboor(checkColor, j->first, graph, mymap)) { // we check that this vertex has any connection with other vertexes which we colored them and they are located in checkColor.
                    colorTable[mymap[j->first]] = color;
                    checkColor.push_back(j->first);
                }
            }
        }
        checkColor.clear(); // we should clear vector for each new color process.
        color = static_cast<colorList>(static_cast<int>(color) + 1u); // we should increase color as we start new color process
    }


    /*for (auto i = myvec.begin(); i != myvec.end(); ++i) {
        std::cout << "Ders Adi : " << i->first << " ID : " << mymap[i->first] << " Color : " << colorTable[mymap[i->first]] << "\n";
    }*/

    return colorTable;
}

int totalNumberOfStudents(vector<classLists>& classes, string className)
{
    auto count{ 0 };
    for (auto& i : classes) {
        if (i.courseID == className) {
            ++count;
        }
    }
    return count;
}

/*
* This function is implemented to read room information’s and store them in a vector. We provided an example input file and 
  this input file should be located in the same path with .exe or if our code will be compiled again,
  it should be located in the project folder. Also, csv file name should be same since code implementation is done considering its name
*/
times calculatingExamTimesforClass(vector<vector<vector<int>>>& room_time_list, const std::vector<roomList>& rooms, pair<int, int> duration, int numberOfStudents, int* day, int *hour, string className,int* sayac)
{
    if ((*hour < 9) || (*hour > 18) || (*day < 0) || (*day > 6))
        throw myException{};

    if (rooms.size() > theNumberOfRooms) {
        cout << "Room Sayisini Belirtin\n";
        throw myException{};
    }
    
    /*
    * This vector object will store room information where relevant class exam will be there.
    */
    vector<string> vec;

    int temp_hour{ *hour };
    int temp_hour_2{ *hour };
    auto duration_2{ duration };
    numberOfStudents *= 2;
    int count{};

    /*
    * Each room will be checked for exam and if it is available, then it will be booked.
    */
    for (auto i = rooms.begin() + ( * sayac); i != rooms.end(); ++i) {
        (*sayac)++;
        while (duration.first || duration.second) {
            if (room_time_list[distance(rooms.begin(), i)][*day][*hour - 9] == 0) {
                if (duration.first)
                    --duration.first;
                else if (duration.second)
                    duration.second = 0;
                room_time_list[distance(rooms.begin(), i)][*day][*hour - 9] = 1;
                ++(* hour);
            }
            else {
                while (temp_hour < *hour) {
                    room_time_list[distance(rooms.begin(), i)][*day][temp_hour - 9] = 0;
                    ++temp_hour;
                }
                temp_hour = temp_hour_2;
                *hour = temp_hour;
                duration = duration_2;
                break;
            }
            if (*hour > 18) {
                while (temp_hour < *hour) {
                    room_time_list[distance(rooms.begin(), i)][*day][temp_hour - 9] = 0;
                    ++temp_hour;
                }
                count = 0;
                i = rooms.begin();
                *sayac = 0;
                temp_hour_2 = 9;
                temp_hour = temp_hour_2;
                *hour = temp_hour_2;
                ++(* day);
                if (*day > 6)
                    throw myException{};
                duration = duration_2;
                vec.clear();
            }
        } 

        if (temp_hour != *hour) {
            count += i->capacity;
            vec.emplace_back(i->roomID);
        }

        temp_hour = temp_hour_2;
        *hour = temp_hour;
        duration = duration_2;

        if (count > numberOfStudents) {
            break;
        }
            
      
    }

    if (count < numberOfStudents) {
        vec.clear();
        vec.emplace_back("Sinav Olusturalamadi");
        return { className,day_of_array[*day],0, 0, 0,0, vec };
    }
        

    return { className,day_of_array[*day], temp_hour_2, 0, temp_hour_2 + duration_2.first, duration_2.second, vec };

}

/*
*   This function is implemented to determine examination times for each classes. While performing that, it is using assigned ID’s for classes, 
*   generated graph structure,assigned colors, roomID’s and their capacities. Each examination time is stored in a vector as a times class 
*   and this times class has class name, examination day, start hour, start min, finish hour, finish min, rooms members.
*/
vector<times> createExamSchedule(myGraph& graph, vector<classLists>& classes, std::vector<roomList>& rooms, map<string, int>& mapForClassandID, map<int, colorList>& mapForClassIDandColor)
{
    /*
    * It stores all classes as ID in key and their exam duration as pair in value.
    */
    map<int, pair<int, int>> duration{};

    /*
    * It store all classes as ID in key and how many students takes this class in value.
    */
    map<int, int> numberOfStudents{};

    /*
    * This objects will be used to calculate exact day and hour for a class.
    */
    int day{ 0 };
    int hour{ 9 };
    int sayac{};
    int hourForRefresh{};
    int minForRefresh{};
    int dayForRefresh{};

    /*
    * This vector object will store exam time information for each class.
    */
    vector<times> examTimeList;
    examTimeList.reserve(mapForClassandID.size());

    /*
    * It is used for filling numberOfStudents.
    */
    for (auto i = mapForClassandID.begin(); i != mapForClassandID.end(); ++i) {
        numberOfStudents[i->second] = totalNumberOfStudents(classes, i->first);
    }

    /*
    * It is used for filling duration.
    */
    for (auto i = classes.begin(); i != classes.end(); ++i) {
        duration[mapForClassandID[i->courseID]] = pair<int, int>{ i->duration / 60, i->duration % 60 };
    }

    /*
    * Specific exam time for each class is calculated for loops located below. Each class has a color and
    * examination time  for classes having same color will be at the same time.
    */
    for (colorList i = white; i <= color_11; i = static_cast<colorList>(static_cast<int>(i) + 1)) {
        if (day != dayForRefresh) {
            hourForRefresh = minForRefresh = 0;
        }
        for (auto j = mapForClassandID.begin(); j != mapForClassandID.end(); ++j) {
            if (mapForClassIDandColor[j->second] == i) {
                dayForRefresh = day;

                if (sayac > rooms.size()) {
                    vector<string> temp_vec{ "Sinav Olusturulamadi" };
                    times s(j->first, "none"s, 0, 0, 0, 0, temp_vec);
                    examTimeList.emplace_back(s);
                    sayac = 0;
                    continue;
                }

                /*
                * calculatingExamTimesforClass is a function calculating exact exam time according to our restriction and returns is as a times object and then
                * this times object is pushed to our examTimeList vector.
                */
                examTimeList.emplace_back(calculatingExamTimesforClass(room_time_list, rooms, duration[j->second], numberOfStudents[j->second], &day, &hour, j->first, &sayac));
                
                if (hourForRefresh < examTimeList.back().finish_hour && day == dayForRefresh) {
                    hourForRefresh = examTimeList.back().finish_hour;
                    minForRefresh = examTimeList.back().finish_min;
                }else if(hourForRefresh ==  examTimeList.back().finish_hour && minForRefresh < examTimeList.back().finish_min && day == dayForRefresh) {
                    minForRefresh = examTimeList.back().finish_min;
                }else if (day != dayForRefresh) {
                    hourForRefresh = examTimeList.back().finish_hour;
                    minForRefresh = examTimeList.back().finish_min;
                    dayForRefresh = day;
                }
            }
        }
        sayac = 0;

        hour = hourForRefresh + 1;

        if (hour > 18) {
            hourForRefresh = 0;
            minForRefresh = 0;
            hour = 9;
            ++day;
        }

        if (day > 6)
            return examTimeList;
    }

    return examTimeList;
}

/*
*   This function is implemented to book a room.
*   
    Please giving room value using RoomLists.csv file. If you assign differen room name, it will not work. Also, it is true for day. It should be assigned following.
    static string day_of_array[]{ "monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday" };

    For example, room could be “A101” as it is located in RoomLists.csv file and also day could be “Monday” according to day_of_array object. 

    duration argument should be how many hours room is booked.

    Last point, if you assigned an time off hour for booking, it will not run.
*/
void bookRoom(const char* room, const char* day, int start_hour, int start_min, int duration)
{
    if ((start_hour > 18) || (start_hour < 9))
        return;

    if ((start_hour + duration) > 18)
        return;

    int count{};
    int roomID{7};
    int dayID{7};
    int d{ duration };

    if ((start_min) && ((start_hour + duration) < 18))
        ++duration;

    for (auto& i : roomListVec) {
        if (i.roomID == room) {
            roomID = count;
            break;
        } 
        ++count;
    }

    count = 0;

    for (int i = 0; i < 7; ++i) {
        if (day_of_array[i] == day) {
            dayID = count;
            break;
        }
        ++count;
    }

    if ((roomID >= roomListVec.size()) || (dayID == 7))
        return;

    bookHours.emplace_back(room, dayID, start_hour, start_min, d, start_min);

    while (duration--) {
        room_time_list[roomID][dayID][start_hour++ - 9] = 1;
    }
}

/*
* This function is used to write examination times to csv file.
*/
void writeToCSV(const string& filename, const vector<times>& examScheduleResult) {
    std::ofstream file(filename);


    // Write data
    string examTimeforCsv{};


    // Write CSV header
    file << "----------MONDAY-------------\n";

    for (const auto& d : examScheduleResult) {
        if (d.day == day_of_array[0]) {
            examTimeforCsv = to_string(d.start_hour) + ":" + to_string(d.start_min) + " - " + to_string(d.finish_hour) + ":" +
                to_string(d.finish_min) + "     " + d.className + " -  Rooms:  ";
            for (const auto& d1 : d.rooms) {
                examTimeforCsv += (d1 + "; ");
            }
            file << examTimeforCsv << "\n";
        }
        else break;

    }

    file << "----------TUESDAY-------------\n";
    for (const auto& d : examScheduleResult) {
        if (d.day == day_of_array[1]) {
            examTimeforCsv = to_string(d.start_hour) + ":" + to_string(d.start_min) + " - " + to_string(d.finish_hour) + ":" +
                to_string(d.finish_min) + "     " + d.className + " -  Rooms:  ";
            for (const auto& d1 : d.rooms) {
                examTimeforCsv += (d1 + "; ");
            }
            file << examTimeforCsv << "\n";
        }
        

        
    }
    file << "----------WEDNESDAY-------------\n";
    for (const auto& d : examScheduleResult) {
        if (d.day == day_of_array[2]) {
            examTimeforCsv = to_string(d.start_hour) + ":" + to_string(d.start_min) + " - " + to_string(d.finish_hour) + ":" +
                to_string(d.finish_min) + "     " + d.className + " -  Rooms:  ";
            for (const auto& d1 : d.rooms) {
                examTimeforCsv += (d1 + "; ");
            }
            file << examTimeforCsv << "\n";
        }
        

    }

    file << "----------THURSDAY-------------\n";
    for (const auto& d : examScheduleResult) {
        if (d.day == day_of_array[3]) {
            examTimeforCsv = to_string(d.start_hour) + ":" + to_string(d.start_min) + " - " + to_string(d.finish_hour) + ":" +
                to_string(d.finish_min) + "     " + d.className + " -  Rooms:  ";
            for (const auto& d1 : d.rooms) {
                examTimeforCsv += (d1 + "; ");
            }
            file << examTimeforCsv << "\n";
        }
    }

    file << "----------FRIDAY-------------\n";
    for (const auto& d : examScheduleResult) {
        if (d.day == day_of_array[4]) {
            examTimeforCsv = to_string(d.start_hour) + ":" + to_string(d.start_min) + " - " + to_string(d.finish_hour) + ":" +
                to_string(d.finish_min) + "     " + d.className + " -  Rooms:  ";
            for (const auto& d1 : d.rooms) {
                examTimeforCsv += (d1 + "; ");
            }
            file << examTimeforCsv << "\n";
        }

    }

    file << "----------SATURDAY-------------\n";
    for (const auto& d : examScheduleResult) {
        if (d.day == day_of_array[5]) {
            examTimeforCsv = to_string(d.start_hour) + ":" + to_string(d.start_min) + " - " + to_string(d.finish_hour) + ":" +
                to_string(d.finish_min) + "     " + d.className + " -  Rooms:  ";
            for (const auto& d1 : d.rooms) {
                examTimeforCsv += (d1 + "; ");
            }
            file << examTimeforCsv << "\n";
        }

    }

    file << "----------SUNDAY-------------\n";
    for (const auto& d : examScheduleResult) {
        if (d.day == day_of_array[6]) {
            examTimeforCsv = to_string(d.start_hour) + ":" + to_string(d.start_min) + " - " + to_string(d.finish_hour) + ":" +
                to_string(d.finish_min) + "     " + d.className + " -  Rooms:  ";
            for (const auto& d1 : d.rooms) {
                examTimeforCsv += (d1 + "; ");
            }
            file << examTimeforCsv << "\n";
        }
 

    }

    file << "\n\n-------bookedHours--------\n";
    file << "-------Day----------,----------hour----------,----------rooms-----------------\n";

    for (auto& i : bookHours) {
        file << day_of_array[get<1>(i)] << "," << "from " << get<2>(i) << " : " << get<3>(i) << " to "
            << get<2>(i) + get<4>(i) << " : " << get<5>(i) << "," << get<0>(i) << "\n";
    }

    file << "\n";

    file.close();
}

