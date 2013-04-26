// Ray Tracer for 3D sound rendering
// Shipeng Xu 2013
// billhsu.x@gmail.com
// Shanghai University

#include "hrtf.h"
#include "wav.h"

hrtf::hrtf(std::string dir)
{
    load(dir);
}
hrtf::~hrtf()
{
    for(int i=0; i<count(hrtf_list.size()); ++i)
    {
        free(hrtf_list[i].ir);
    }
}
void hrtf::load(std::string dir)
{
    WIN32_FIND_DATA FindData;
    HANDLE hError;
    int FileCount = 0;
    char FilePathName[LEN];
    char FullPathName[LEN];
    strcpy(FilePathName, Path);
    strcat(FilePathName, "\\*.*");
    hError = FindFirstFile(FilePathName, &FindData);
    if (hError == INVALID_HANDLE_VALUE)
    {
        printf("Failed!");
        return 0;
    }
    while(::FindNextFile(hError, &FindData))
    {
        if (strcmp(FindData.cFileName, ".") == 0 
                || strcmp(FindData.cFileName, "..") == 0 )
        {
            continue;
        }

        wsprintf(FullPathName, "%s\\%s", Path,FindData.cFileName);
        FileCount++;
        printf("\n%d  %s  ", FileCount, FullPathName);

        if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            printf("<Dir>");
            DirectoryList(FullPathName);
        }
        else
        {
            read_hrtf(FullPathName);
        }

    }
}

void hrtf::read_hrtf(std::string filename)
{
    //TODO:split h e
    long fileSize;
    hrtf_data data;
    data.ir = readWavFileData(filename.c_str(), fileSize);
    data.h = h;
    data.e = e;
    hrtf_list.push_back(data);

}
short* hrtf::getHRTF(RayTracer::vector3 direction)
{
    float yaw = atan2(direction.y, direction.x)*180.0f/PI;
    float pitch = atan2(direction.z, 
            sqrt(direction.x*direction.x+direction.y*direction.y))*180.0f/PI;
    float min_dist = 1000.0f;
    int h,e;
    for(int i=0; i<hrtf_list.size(); ++i)
    {
        if(hrtf_list[i].h-pitch<min_dist)
        {
            min_dist = hrtf_list[i].h-pitch;
            h = hrtf_list[i].h;
        }
    }

    min_dist = 1000.0f;
    short* target_ir = null;
    for(int i=0; i<hrtf_list.size(); ++i)
    {
        if(hrtf_list[i].h != h) continue;
        if(hrtf_list[i].e-yaw<min_dist)
        {
            min_dist = hrtf_list[i].e-yaw;
            e = hrtf_list[i].e;
            target_ir = hrtf_list[i].ir;
        }
    }

    return target_ir;


}
