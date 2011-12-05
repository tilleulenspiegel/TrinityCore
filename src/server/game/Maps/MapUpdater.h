#ifndef _MAP_UPDATER_H_INCLUDED
#define _MAP_UPDATER_H_INCLUDED

#include <ace/Thread_Mutex.h>
#include <ace/Condition_Thread_Mutex.h>

#include "DelayExecutor.h"
#include "Common.h"

class Map;
struct MapID;

struct MapBrokenData
{
    explicit MapBrokenData()
    {
        Reset();
    }

   void Reset()
    {
        count = 1;
        lastErrorTime = time(NULL);
    };

    void IncreaseCount() { ++count; lastErrorTime = time(NULL);};
    uint32 count;
    time_t lastErrorTime;
;

typedef std::map<ACE_thread_t const, MapID> ThreadMapMap;
typedef std::map<ACE_thread_t const, uint32/*MSTime*/>  ThreadStartTimeMap;
typedef std::map<MapID,MapBrokenData> MapBrokenDataMap;

class MapUpdater
{
    public:

        MapUpdater();
        virtual ~MapUpdater();

        friend class MapUpdateRequest;

        int schedule_update(Map& map, ACE_UINT32 diff);

        int wait();

        int activate(size_t num_threads);

        int deactivate();

        bool activated();

        void update_finished();

        void register_thread(ACE_thread_t const threadId, uint32 mapId, uint32 instanceId);
        void unregister_thread(ACE_thread_t const threadId);

        MapID const* GetMapPairByThreadId(ACE_thread_t const threadId);
        void FreezeDetect();

        void SetBroken( bool value = false) { m_broken = value; };
        bool IsBroken() { return m_broken; };
        void ReActivate( uint32 threads);

        void MapBrokenEvent(MapID const* mapPair);
        MapBrokenData const* GetMapBrokenData(MapID const* mapPair);

    private:

        DelayExecutor m_executor;
        ACE_Thread_Mutex m_mutex;
        ACE_Condition_Thread_Mutex m_condition;
        size_t pending_requests;

        ThreadMapMap m_threads;
        ThreadStartTimeMap m_starttime;
        MapBrokenDataMap   m_brokendata;
        bool m_broken;
};

#endif //_MAP_UPDATER_H_INCLUDED
