#ifndef ALOHA_STATS_H
#define ALOHA_STATS_H

// Used to avoid cereal compilation issues on iOS/MacOS when check() macro is defined.
#ifdef __APPLE__
#define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES 0
#endif

#include <string>
#include <map>

#include "http_client.h"
#include "logger.h"
#include "message_queue.h"
#include "event_base.h"

#include "cereal/include/archives/binary.hpp"
#include "cereal/include/types/string.hpp"
#include "cereal/include/types/map.hpp"

namespace aloha {

class StatsUploader {
 public:
  explicit StatsUploader(const std::string& url) : url_(url) {
  }
  void OnMessage(const std::string& message, size_t /*unused_dropped_events*/ = 0) const {
    // TODO(AlexZ): temporary stub.
    HTTPClientPlatformWrapper(url_).set_post_body(message, "application/alohalytics-binary-blob").RunHTTPRequest();
  }
  const std::string& GetURL() const {
    return url_;
  }

 private:
  const std::string url_;
};

typedef std::map<std::string, std::string> TStringMap;

class Stats {
  StatsUploader uploader_;
  mutable MessageQueue<StatsUploader> message_queue_;
  const std::string storage_path_;
  // TODO: Use this id for every file sent to identify received files on the server.
  const std::string installation_id_;
  const bool use_message_queue_;
  bool debug_mode_ = false;

 public:
  // @param[in] statistics_server_url where we should send statistics.
  // @param[in] storage_path_with_a_slash_at_the_end where we aggregate our events.
  // @param[in] installation_id some id unique for this app installation.
  Stats(const std::string& statistics_server_url,
        const std::string& storage_path_with_a_slash_at_the_end,
        const std::string& installation_id,
        bool use_message_queue = true)
      : uploader_(statistics_server_url),
        message_queue_(uploader_),
        storage_path_(storage_path_with_a_slash_at_the_end),
        installation_id_(installation_id),
        use_message_queue_(use_message_queue) {
  }

  void LogEvent(std::string const& event_name) const {
    if (debug_mode_) {
      LOG("LogEvent:", event_name);
    }
    AlohalyticsKeyEvent event;
    event.key = event_name;
    std::ostringstream sstream;
    {
      cereal::BinaryOutputArchive(sstream) << event;
    }
    PushMessageViaQueue(sstream.str());
  }

  void LogEvent(std::string const& event_name, std::string const& event_value) const {
    if (debug_mode_) {
      LOG("LogEvent:", event_name, "=", event_value);
    }
    AlohalyticsKeyValueEvent event;
    event.key = event_name;
    event.value = event_value;
    std::ostringstream sstream;
    {
      cereal::BinaryOutputArchive(sstream) << event;
    }
    PushMessageViaQueue(sstream.str());
  }

  void LogEvent(std::string const& event_name, TStringMap const& value_pairs) const {
    if (debug_mode_) {
      LOG("LogEvent:", event_name, "=", value_pairs);
    }
    AlohalyticsKeyPairsEvent event;
    event.key = event_name;
    event.pairs = value_pairs;
    std::ostringstream sstream;
    {
      cereal::BinaryOutputArchive(sstream) << event;
    }
    PushMessageViaQueue(sstream.str());
  }

  void DebugMode(bool enable) {
    debug_mode_ = enable;
    if (enable) {
      LOG("Alohalytics: Enabled debug mode.");
      LOG("Alohalytics: Server url:", uploader_.GetURL());
      LOG("Alohalytics: Storage path:", storage_path_);
      LOG("Alohalytics: Installation ID:", installation_id_);
    }
  }

  // Forcedly tries to upload all stored records to the server.
  void Upload() {
    if (debug_mode_) {
      LOG("Alohalytics: Uploading data to", uploader_.GetURL());
    }
    // TODO
  }

  // TODO(dkorolev): Was needed for unit testing.
  int UniversalDebugAnswer() const {
    return 42;
  }

 private:
  void PushMessageViaQueue(const std::string& message) const {
    if (!use_message_queue_) {
      // Blocking call, waiting in the calling thread to complete it.
      uploader_.OnMessage(message);
    } else {
      // Asynchronous call, returns immediately.
      message_queue_.PushMessage(message);
    }
  }
};

}  // namespace aloha

#endif  // #ifndef ALOHA_STATS_H