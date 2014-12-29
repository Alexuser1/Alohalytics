#ifndef STATS_UPLOADER_H
#define STATS_UPLOADER_H

#include <string>

#include "http_client.h"
#include "FileStorageQueue/fsq.h"
#include "logger.h"

namespace aloha {

// Collects statistics and uploads it to the server.
class StatsUploader final {
 public:
  explicit StatsUploader(const std::string& upload_url, const std::string installation_id_event)
      : upload_url_(upload_url), installation_id_event_(installation_id_event) {}

  template <typename T_TIMESTAMP>
  fsq::FileProcessingResult OnFileReady(const fsq::FileInfo<T_TIMESTAMP>& file, T_TIMESTAMP /*now*/) {
    if (debug_mode_) {
      LOG("Alohalytics: trying to upload statistics file", file.full_path_name, "to", upload_url_);
    }
    HTTPClientPlatformWrapper request(upload_url_);
    // Append unique installation id in the beginning of each file sent to the server.
    // TODO(AlexZ): Refactor fsq to silently add it in the beginning of each file.
    // TODO*(AlexZ): Use special compact/fixed size? format for it when cereal will be replaced.
    {
      std::ifstream fi(file.full_path_name, std::ifstream::in | std::ifstream::binary);
      fi.seekg(0, std::ios::end);
      const size_t file_size = fi.tellg();
      fi.seekg(0);
      std::string buffer(installation_id_event_);
      buffer.resize(buffer.size() + file_size);
      fi.read(&buffer[installation_id_event_.size()], file_size);
      if (fi.good()) {
        request.set_post_body(std::move(buffer), "application/alohalytics-binary-blob");
      } else if (debug_mode_) {
        LOG("Alohalytics: can't load file with size", file_size, "into memory");
        return fsq::FileProcessingResult::FailureNeedRetry;
      }
    }
    bool success = false;
    try {
      success = request.RunHTTPRequest();
    } catch (const std::exception& ex) {
      if (debug_mode_) {
        LOG("Alohalytics: exception while trying to upload file", ex.what());
      }
    }
    if (success) {
      if (debug_mode_) {
        LOG("Alohalytics: file was successfully uploaded.");
      }
      return fsq::FileProcessingResult::Success;
    } else {
      if (debug_mode_) {
        LOG("Alohalytics: error while uploading file", request.error_code());
      }
      return fsq::FileProcessingResult::FailureNeedRetry;
    }
  }

  const std::string& GetURL() const { return upload_url_; }

  void set_debug_mode(bool debug_mode) { debug_mode_ = debug_mode; }

 private:
  const std::string upload_url_;
  // Already serialized event with unique installation id.
  const std::string installation_id_event_;
  bool debug_mode_ = false;
};

}  // namespace aloha

#endif  // STATS_UPLOADER_H
