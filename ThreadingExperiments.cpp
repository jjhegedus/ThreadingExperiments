#include "pch.h"

#include <thread>

#include <sstream>
#include <time.h>
#include <ctime>
#include <string>

#include "PrioritizedStagedProcessor.h"
//#include "Scheduler.h"
//#include "ItemProcessor.h"
#include "ProcessorGroup.h"
#include "NamedItemStore.h"


void TestPrioritizedStagedProcessing() {
  std::vector<ndtech::PrioritizedProcessingStage<std::string>> stages{
  ndtech::PrioritizedProcessingStage(
    "Stage1",
    std::function<std::string(std::string)>([](std::string item) {
      std::cout << "Stage1 Processing: item = " << item << std::endl;
      return item;
    })),

  ndtech::PrioritizedProcessingStage(
    "Stage2",
    std::function<std::string(std::string)>([](std::string item) {
      std::cout << "Stage2 Processing: item = " << item << std::endl;
      return item;
    })),

  ndtech::PrioritizedProcessingStage(
    "Stage3",
    std::function<std::string(std::string)>([](std::string item) {
      std::cout << "Stage3 Processing: item = " << item << std::endl;
      return item;
    }))
  };

  stages[0].m_items.push_back("test0.0");
  stages[0].m_items.push_back("test0.1");
  stages[1].m_items.push_back("test1.0");
  stages[1].m_items.push_back("test1.1");
  auto processing = ndtech::CreatePrioritizedStagedProcessor(stages);
  std::cout << "PrioritizedStagedProcessing created" << std::endl;
  processing.Start();


  std::string line;
  while (std::getline(std::cin, line))
  {
    if (line == "q") {
      std::cout << "Received quit signal" << std::endl;
      break;
    }
    if (line == "a") {
      std::cout << "Adding Item:  Enter Item Name: ";
      std::getline(std::cin, line);
      std::cout << "Adding Item: " << line << std::endl;
      processing.AddItem(line);
    }
    else {
      std::cout << "Read '" << line << "' from stdin" << std::endl;
    }
  }
  std::cout << "Exited UI loop" << std::endl;



  processing.Join();
  std::cout << "Processing thread joined. UI thread preparing to exit" << std::endl;
}

void TestProcessorGroup() {
  std::vector<ndtech::PrioritizedProcessingStage<std::string>> stages;
  stages.emplace_back("PrioritizedProcessingStage1:Stage1",
    std::function<std::string(std::string)>([](std::string item) {
      std::cout << "Stage1.1 Processing: item = " << item << std::endl;
      std::cout << "Stage1.1 Processing: threadId = " << std::this_thread::get_id() << std::endl;
      return item;
      }));
  stages.emplace_back("PrioritizedProcessingStage1:Stage2",
    std::function<std::string(std::string)>([](std::string item) {
      std::cout << "Stage1.2 Processing: item = " << item << std::endl;
      std::cout << "Stage1.2 Processing: threadId = " << std::this_thread::get_id() << std::endl;
      return item;
      }));
  stages.emplace_back(
    "PrioritizedProcessingStage1:Stage3",
    std::function<std::string(std::string)>([](std::string item) {
      std::cout << "Stage1.3 Processing: item = " << item << std::endl;
      std::cout << "Stage1.3 Processing: threadId = " << std::this_thread::get_id() << std::endl;

      return item;
      }));

  stages[0].m_items.emplace_back("test0.0");
  stages[0].m_items.emplace_back("test0.1");
  stages[1].m_items.emplace_back("test1.0");
  stages[1].m_items.emplace_back("test1.1");
  auto processing = ndtech::CreatePrioritizedStagedProcessor(stages);
  std::cout << "PrioritizedStagedProcessing created" << std::endl;


  std::vector<ndtech::PrioritizedProcessingStage<std::string>> stages2;
  stages2.emplace_back("PrioritizedProcessingStage2:Stage1",
    std::function<std::string(std::string)>([](std::string item) {
      std::cout << "Stage2.1 Processing: item = " << item << std::endl;
      std::cout << "Stage2.1 Processing: threadId = " << std::this_thread::get_id() << std::endl;
      return item;
      }));
  stages2.emplace_back("PrioritizedProcessingStage2:Stage2",
    std::function<std::string(std::string)>([](std::string item) {
      std::cout << "Stage2.2 Processing: item = " << item << std::endl;
      std::cout << "Stage2.2 Processing: threadId = " << std::this_thread::get_id() << std::endl;
      return item;
      }));
  stages2.emplace_back(
    "PrioritizedProcessingStage2:Stage3",
    std::function<std::string(std::string)>([](std::string item) {
      std::cout << "Stage2.3 Processing: item = " << item << std::endl;
      std::cout << "Stage2.3 Processing: threadId = " << std::this_thread::get_id() << std::endl;

      return item;
      }));

  stages2[0].m_items.emplace_back("test2.0.0");
  stages2[0].m_items.emplace_back("test2.0.1");
  stages2[1].m_items.emplace_back("test2.1.0");
  stages2[1].m_items.emplace_back("test2.1.1");
  auto processing2 = ndtech::CreatePrioritizedStagedProcessor(stages2);
  std::cout << "PrioritizedStagedProcessing2 created" << std::endl;


  ndtech::ProcessorGroup processorGroup(processing, processing2);
  processorGroup.Start();

  std::string line;
  while (std::getline(std::cin, line))
  {
    if (line == "q") {
      std::cout << "Received quit signal" << std::endl;
      break;
    }
    if (line == "a") {
      std::cout << "Adding Item:  Enter Item Name: ";
      std::getline(std::cin, line);
      std::cout << "Adding Item: " << line << std::endl;
      processing.AddItem(line);
    }
    else {
      std::cout << "Read '" << line << "' from stdin" << std::endl;
    }
  }
  std::cout << "Exited UI loop" << std::endl;



  processorGroup.Join();
  std::cout << "Processing thread joined. UI thread preparing to exit" << std::endl;
}

void TestNamedItemStore() {
  ndtech::NamedItemStore<std::string> store(
    [](std::pair<std::string, std::string> item) {
      std::cout << "Processing namedItem : " << item.first << std::endl;
      std::this_thread::sleep_for(2s); 
      return item; 
    });

  store.AddItem("item1", "i1");
  store.AddItem("item2", "i2");

  store.Start();
  

  std::string line;
  while (std::getline(std::cin, line))
  {
    if (line == "q") {
      std::cout << "Received quit signal" << std::endl;
      break;
    }
    if (line == "a") {
      std::cout << "Adding Item:  Enter Item Name: ";
      std::string newName;
      std::getline(std::cin, newName);
      std::cout << "Adding Item:  Enter Item value: ";
      std::string newValue;
      std::getline(std::cin, newValue);
      std::cout << "Adding Item: NAME = " << newName << " VALUE = " << newValue << std::endl;
      store.AddItem(newName, newValue);
    }
    if (line == "g") {
      std::cout << "Getting Item:  Enter Item Name: ";
      std::string itemName;
      std::getline(std::cin, itemName);
      std::cout << "Getting Item: NAME = " << itemName << std::endl;
      std::string str = store.GetItem(itemName);
      std::cout << "Got Item: NAME = " << itemName << " VALUE = " << str << std::endl;
    }

    else {
      std::cout << "Read '" << line << "' from stdin" << std::endl;
    }
  }
  std::cout << "Exited UI loop" << std::endl;

  std::string item1Result = store.GetItem("item1");
  std::cout << "Got item1" << std::endl;

  store.Join();
  std::cout << "Processing thread joined. UI thread preparing to exit" << std::endl;
}

void InitializeNamedItemStore(ndtech::NamedItemStore<std::string>* store) {


  store->AddItem("item1", "i1");
  store->AddItem("item2", "i2");

  store->Start();
}

template <typename ItemType>
void AddItemToStore(ndtech::NamedItemStore<ItemType>* store, std::string name, ItemType item) {
  std::thread thread = std::thread{ [store, name, item]() { store->AddItem(name, item); } };

  thread.join();
}

int main()
{
  // This is the ui thread
  // all background work must happen on background tasks

  std::cout << "main threadId = " << std::this_thread::get_id() << std::endl;

  //TestPrioritizedStagedProcessing();
  //TestProcessorGroup();

  ndtech::NamedItemStore<std::string> store(
    [](std::pair<std::string, std::string> item) {
      std::cout << "Processing namedItem : " << item.first << std::endl;
      std::this_thread::sleep_for(2s);
      return item;
    });

  InitializeNamedItemStore(&store);

  std::string line;
  while (std::getline(std::cin, line))
  {
    if (line == "q") {
      std::cout << "Received quit signal" << std::endl;
      break;
    }
    if (line == "a") {
      std::cout << "Adding Item:  Enter Item Name: ";
      std::string newName;
      std::getline(std::cin, newName);
      std::cout << "Adding Item:  Enter Item value: ";
      std::string newValue;
      std::getline(std::cin, newValue);
      std::cout << "Adding Item: NAME = " << newName << " VALUE = " << newValue << std::endl;
      store.AddItem(newName, newValue);
      //AddItemToStore(&store, newName, newValue);
    }
    if (line == "g") {
      std::cout << "Getting Item:  Enter Item Name: ";
      std::string itemName;
      std::getline(std::cin, itemName);
      std::cout << "Getting Item: NAME = " << itemName << std::endl;
      std::string str = store.GetItem(itemName);
      std::cout << "Got Item: NAME = " << itemName << " VALUE = " << str << std::endl;
    }

    else {
      std::cout << "Read '" << line << "' from stdin" << std::endl;
    }
  }
  std::cout << "Exited UI loop" << std::endl;

  std::string item1Result = store.GetItem("item1");
  std::cout << "Got item1" << std::endl;

  store.Join();
  std::cout << "Processing thread joined. UI thread preparing to exit" << std::endl;

  return 0;
}


//#include <string>
//#define NONIUS_RUNNER
//#include <nonius/nonius.h++>
//#include <nonius/main.h++>
//#include <iostream>
//
//using namespace std;
//
//string short_string = "hello";
//string long_string("0123456789abcdefghijklmnopqrstuvwxyz");
//
//void Test1() {
//  std::cout << "Test1";
//}
//
////NONIUS_BENCHMARK("StringCopyShort", []
////  {
////    string copy(short_string);
////  })
////
////  NONIUS_BENCHMARK("StringCopyLong", []
////    {
////      string copy(long_string);
////    })
//
//
//NONIUS_BENCHMARK("Test1", []
//  {
//    Test1();
//  })