#include "txtParse.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <vector>

// #define NUMOFSEEDS 30
#define NUMOFSEEDS 5
// #define NUMOFREQUESTS 100000
#define NUMOFREQUESTS 30000
#define LTDOFSEG      8
#define MAXTHREADS    4
// #define LTDOFSEG 1
#define MU 0.2

using namespace std;

void
thread_function(string cmd)
{
  system(cmd.c_str());
  // cout << cmd << endl;
}

int
main()
{
  // For FuFVFMs
  vector<int>         Core, Lambda, Seed, Erlang;
  double              Number;
  static unsigned int NumofRequests = NUMOFREQUESTS;
  // static string Exec = "/Users/mt1ger/Code_Projects/Simulator/"
  //                      "LPS_SEG_SimSDM-EON/Sim_Firstfit";
  // static string Topo = "/Users/mt1ger/Code_Projects/Simulator/"
  //                      "LPS_SEG_SimSDM-EON/Topology/NSF14.txt";
  // static string Path = "/Users/mt1ger/Code_Projects/Simulator/"
  //                      "LPS_SEG_SimSDM-EON/Plot_Firstfit/";

  static string Exec = "/home/mt1ger/"
                       "woLPS_SEG_SimSDM-EON/Sim_Firstfit";
  static string Topo = "/home/mt1ger/"
                       "woLPS_SEG_SimSDM-EON/Topology/US24.txt";
  static string Path = "/home/mt1ger/"
                       "woLPS_SEG_SimSDM-EON/Plot_Firstfit/";

  unsigned int   cnt = 0;
  string         Filename;
  vector<string> FolderSet;

  Core.push_back(1);
  /* Core.push_back(2); */
  /* Core.push_back(4); */
  Core.push_back(7);

  for(int i = 2; i < 143; i += 2) {
    Lambda.push_back(i);
  }

  for(int i = 0; i < Lambda.size(); i += 1)
  {
    Erlang.push_back(Lambda[i] / MU);
  }

  // cout << "Input a number for seed and press enter: " << endl;
  // cin >> Number;
  Number = 40;
  srand(Number);
  for(int i = 0; i < NUMOFSEEDS; i++)
  {
    Seed.push_back(rand() / 65535);
  }

  vector<vector<string>> cmds;
  vector<string>         filePaths;

  for(unsigned int Max = 1; Max <= LTDOFSEG; Max *= 2)
  {
    string IsoFolder, Command, PlotFile;

    for(unsigned int k = 0; k < Core.size(); k++)
    {
      IsoFolder = Path + to_string(Max) + "/Plot" + to_string(Core[k]) + '/';
      Command   = "mkdir -p " + IsoFolder;
      system(Command.c_str());
      filePaths.push_back(IsoFolder);
      PlotFile = IsoFolder + "Plot.txt";
      ofstream       file(PlotFile);
      vector<string> cmds_sec;
      for(unsigned int seed = 0; seed < NUMOFSEEDS; seed++)
      {
        for(unsigned int lambda = 0; lambda < Lambda.size(); lambda++)
        {
          string Cmd = Exec + ' ' + Topo + ' ' + IsoFolder + ' '
                       + to_string(NumofRequests) + ' ' + to_string(Core[k])
                       + ' ' + to_string(Lambda[lambda] * Core[k]) + ' '
                       + to_string(MU) + ' ' + to_string(Seed[seed]) + ' '
                       + to_string(Max);
          cmds_sec.push_back(Cmd);
        }
      }
      cmds.push_back(cmds_sec);
    }
  }


  auto   cntSize             = cmds[0].size();
  int    threadsPerIteration = MAXTHREADS;
  cnt                        = 0;
  while(cnt < cntSize) {
    for(int i = 0; i < cmds.size(); i += threadsPerIteration) {
      thread threads[threadsPerIteration];
      for(int j = 0; j < threadsPerIteration; j++) {
        cout << cmds[i + j][cnt] << endl;
        threads[j] = thread(thread_function, cmds[i + j][cnt]);
      }
      for(int j = 0; j < threadsPerIteration; j++) {
        threads[j].join();
      }
    }
    cnt++;
  }

  for(int i = 0; i < filePaths.size(); i++)
  {
    txtParse txtparse(Core, Erlang, Seed, filePaths[i]);
    txtparse.parse();
  }

  return 1;
}
