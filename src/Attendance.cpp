#include <iostream>
#include <iomanip>
#include <fstream>
#include <utility>
#include <vector>
#include <chrono>
#include <sstream>
#include <string>
#include <ctime>

using namespace std;
using vec = vector<string>;
using matrix = vector<vec>;

//======================================================================

matrix readCSV( string filename )
{
    char separator = ',';
    matrix result;
    string row, item;

    ifstream in( filename );

    while( getline( in, row ) )
    {
        vec R;
        stringstream ss( row );
        while ( getline ( ss, item, separator ) ) R.push_back( item );
        result.push_back( R );
    }

    in.close();
    return result;
}

//======================================================================

void printMatrix( const matrix &M )
{
    for ( vec row : M )
    {
        for ( string s : row ) cout << setw( 22 ) << left << s << " ";    // need a variable format really
        cout << '\n';
    }
}

//======================================================================

string calc_time_5(string time)
{
    size_t colon_pos = time.find(':');
    if (colon_pos != string::npos)
    {
        int hours = stoi(time.substr(0, colon_pos));
        int minutes = stoi(time.substr(colon_pos + 1));
        // Add 5 minutes to the class schedule
        minutes += 6;
        if (minutes >= 60)
        {
            hours += 1;
            minutes -= 60;
        }
        // Format the updated schedule
        stringstream updated_schedule;
        updated_schedule << setw(2) << setfill('0') << hours << ":" << setw(2) << setfill('0') << minutes;
        string updated_schedule_str = updated_schedule.str();

        return updated_schedule_str;
    }
}


//======================================================================

string calc_time_15(string time)
{
    size_t colon_pos = time.find(':');
    if (colon_pos != string::npos)
    {
        int hours = stoi(time.substr(0, colon_pos));
        int minutes = stoi(time.substr(colon_pos + 1));
        // Add 5 minutes to the class schedule
        minutes += 16;
        if (minutes >= 60)
        {
            hours += 1;
            minutes -= 60;
        }
        // Format the updated schedule
        stringstream updated_schedule;
        updated_schedule << setw(2) << setfill('0') << hours << ":" << setw(2) << setfill('0') << minutes;
        string updated_schedule_str = updated_schedule.str();

        return updated_schedule_str;
    }
}


//======================================================================

string getCurrentDateTime() 
{
    auto now = chrono::system_clock::now();
    time_t currentTime = chrono::system_clock::to_time_t(now);
    struct tm* localTimeInfo = localtime(&currentTime);
    // localtime_s(&localTimeInfo, &currentTime); // Use localtime_s for Windows, use localtime for Linux
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTimeInfo);
    return buffer;
}

//======================================================================

void add_date_to_output(matrix &M) 
{
    string currentDate = "Attendance Checked: " + getCurrentDateTime();
    vec newRow = {currentDate};
    M.push_back(newRow);
}

//======================================================================

void check_attendance(matrix &M)
{   
    // Assuming teacherTimeIn is in HH:MM format
    string class_schedule = M[1][0];
    string teacherTimeIn;
    cout << "Class Schedule: " << class_schedule << endl;

    for (vec &row : M)
    {
        if (row.size() == 5 && row[2] == "Teacher")
        {
            teacherTimeIn = row[4]; // Get the student's time in
            cout << "Updating attendance based on teacher's time in: " << teacherTimeIn << endl;

            // Check if the teacher is absent or late
            if (teacherTimeIn.empty() || calc_time_15(class_schedule) < row[4])
            {
                // Teacher is absent, set "N/A" for students
                if (row[4] != "Time")
                    row[4] = "Absent";
            }
            else if (calc_time_5(class_schedule) < row[4])
            {
                if (row[4] != "Time")
                    row[4] = "Late";
            }
            else
            {
                if (row[4] != "Time")
                    row[4] = "Present";
            }
        }
        else if (row.size() == 5)
        {
            string studentTimeIn = row[4];
            // Teacher is on time, check student's time
            if (studentTimeIn.empty() || calc_time_15(teacherTimeIn) < studentTimeIn)
            {
                // Student is absent or late
                if (row[4] != "Time")
                    row[4] = "Absent";
            }
            else if (calc_time_5(teacherTimeIn) < studentTimeIn)
            {
                if (row[4] != "Time")
                    row[4] = "Late";
            }
            else
            {
                // Student is on time
                if (row[4] != "Time")
                    row[4] = "Present";
            }
        }
    }
    cout << "Attendance updated successfully!" << endl;
}

//======================================================================

void writeCSV(const matrix &M, const string& filename)
{
    ofstream out(filename, ios::app); // Open the file in append mode
    if (!out.is_open())
    {
        cerr << "Error: Unable to create file: " << filename << endl;
        return;
    }

    for (const vec &row : M)
    {
        for (size_t i = 0; i < row.size(); ++i)
        {
            out << row[i];
            if (i < row.size() - 1)
                out << ",";
        }
        out << '\n';
    }
    out.close();
}

//======================================================================

int main()
{
    matrix pets = readCSV( "test.csv" );
    printMatrix( pets );

    cout << "\n\n";

    check_attendance( pets ); // check the attendance according to the time the person has timed in.

    add_date_to_output(pets); // add the date the day the attendance was checked.

    printMatrix( pets ); // show the checked attendance.

    writeCSV(pets, "updated_attendance.csv"); // write modified data to a separate file. 
    //Note, if the output file still exist in the path where the output file will be made, the modified data will instead be put in the old output data

    return 0;
}