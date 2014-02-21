#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdio.h>

#include <boost/algorithm/string.hpp>

/*
 * This proof of concept returns a GPFS_STATS object containing the following
 * information about the GPFS node:
 * 
 * name of the GPFS cluster
 * name of the file system
 * number of participating hard disks
 * timestamp of the data
 * bytes read up to that point
 * bytes written up to that point
 * number of open() calls
 * number of close() calls
 * number of readdirs
 * number of inode updates.
 * 
 * It uses the GPFS command "mmpmon" which requires superuser rights.
 */

class GPFS_STATS {
public:
        std::string cluster_;
        std::string fs_;
        unsigned disks_;
        std::string timestamp_;
        unsigned long bytes_r_;
        unsigned long bytes_w_;
        unsigned long opens_;
        unsigned long closes_;
        unsigned long reads_;
        unsigned long writes_;
        unsigned long readdir_;
        unsigned long inode_updates_;

        GPFS_STATS(std::string mmpmon_result) : disks_(0), bytes_r_(0), bytes_w_(0), opens_(0), closes_(0), reads_(0), writes_(0), readdir_(0), inode_updates_(0)
        {
                std::vector<std::string> lines;
                boost::split(lines, mmpmon_result, boost::is_any_of("\n"));

                for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {

                        std::vector<std::string> line;
                        boost::split(line, *it, boost::is_any_of(":"));
                     for (std::vector<std::string>::iterator it2 = line.begin(); it2 != line.end(); ++it2) {

                                if (*it2 == "cluster")
                                        cluster_ = trim(*(++it2));
                                else if (*it2 == "filesystem")
                                        fs_ = trim(*(++it2));
                                else if (*it2 == "disks")
                                        disks_ = atoi(trim(*(++it2)).c_str());
                                else if (*it2 == "timestamp")
                                        timestamp_ = trim(*(++it2)).c_str();
                                else if (*it2 == "read")
                                        bytes_r_ = atoll(trim(*(++it2)).c_str());
                                else if (*it2 == "written")
                                        bytes_w_ = atoll(trim(*(++it2)).c_str());
                                else if (*it2 == "opens")
                                        opens_ = atoll(trim(*(++it2)).c_str());
                                else if (*it2 == "closes")
                                        closes_ = atoll(trim(*(++it2)).c_str());
                                else if (*it2 == "reads")
                                        reads_ = atoll(trim(*(++it2)).c_str());
                                else if (*it2 == "writes")
                                        writes_ = atoll(trim(*(++it2)).c_str());
                                else if (*it2 == "readdir")
                                        readdir_ = atoll(trim(*(++it2)).c_str());
                                else if (*it2 == "inode_updates")
                                        inode_updates_ = atoll(trim(*(++it2)).c_str());
                        }
                }

        }
        void print_stats()
        {
                std::cout << "Cluster: " << cluster_ << std::endl;
                std::cout << "File system: " << fs_ << std::endl;
                std::cout << "Disks: " << disks_ << std::endl;
                std::cout << "Timestamp: " << timestamp_ << std::endl;
                std::cout << "Bytes read: " << bytes_r_ << std::endl;
                std::cout << "Bytes written: " << bytes_w_ << std::endl;
                std::cout << "opens: " << opens_ << std::endl;
                std::cout << "closes: " << closes_ << std::endl;
                std::cout << "reads: " << reads_ << std::endl;
                std::cout << "writes: " << writes_ << std::endl;
                std::cout << "readdirs: " << readdir_ << std::endl;
                std::cout << "inode updates: " << inode_updates_ << std::endl;
        }

private:
        std::string trim(std::string s)
        {
                std::stringstream trimmer;
                std::string retstr;

                trimmer << s;
                trimmer >> retstr;

                return retstr;
        }
};

std::string exec(char *cmd)
{
        FILE *pipe = popen(cmd, "r");

        if (!pipe)
                return "ERROR";

        char buffer[128];
        std::string result = "";

        while (!feof(pipe)) {
                if (fgets(buffer, 128, pipe) != NULL)
                        result += buffer;
        }

        pclose(pipe);

        return result;
}


int main()
{
        GPFS_STATS *g = new GPFS_STATS(exec("/bin/echo fs_io_s | /usr/lpp/mmfs/bin/mmpmon 2> /dev/null"));

        g->print_stats();

        return 0;
}

