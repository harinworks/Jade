#ifndef JADE_OTA_UTIL_H_
#define JADE_OTA_UTIL_H_

#include "../process.h"
#include "../ui.h"
#include "../utils/cbor_rpc.h"
#include <esp_app_format.h>
#include <esp_ota_ops.h>
#include <esp_partition.h>
#include <mbedtls/sha256.h>
#include <stdbool.h>
#include <stddef.h>

#define CUSTOM_HEADER_MIN_WRITE                                                                                        \
    (sizeof(esp_app_desc_t) + sizeof(esp_custom_app_desc_t) + sizeof(esp_image_header_t)                               \
        + sizeof(esp_image_segment_header_t))

// This structure is built into every fw, so we can check downloaded firmware
// is appropriate for the hardware unit we are trying to flash it onto.
// NOTE: For back compat only add to the end of the structure, and increase 'version'
// to indicate those new fields are present.
typedef struct {
    // Version 1 fields
    const uint8_t version;
    const char board_type[32];
    const char features[32];
    const char config[32];

    // Version 2 fields
    // add new fields here
} esp_custom_app_desc_t;

typedef enum { HASHTYPE_FILEDATA, HASHTYPE_FULLFWDATA } hash_type_t;

typedef struct {
    progress_bar_t progress_bar;
    mbedtls_sha256_context* sha_ctx;
    hash_type_t hash_type;
    char id[MAXLEN_ID + 1];
    bool extended_replies;
    const uint8_t* expected_hash;
    const char* expected_hash_hexstr;
    const esp_partition_t* running_partition;
    const esp_partition_t* update_partition;
    esp_ota_handle_t* ota_handle;
    enum ota_status* ota_return_status;
    struct deflate_ctx* dctx;
    const jade_msg_source_t* expected_source;
    bool* validated_confirmed;
    size_t* const remaining_uncompressed;
    size_t remaining_compressed;
    size_t uncompressedsize;
    size_t compressedsize;
    size_t firmwaresize;
    size_t fwwritten;
} jade_ota_ctx_t;

enum ota_status {
    OTA_SUCCESS = 0,
    OTA_ERR_SETUP,
    OTA_ERR_INIT,
    OTA_ERR_BADPARTITION,
    OTA_ERR_DECOMPRESS,
    OTA_ERR_WRITE,
    OTA_ERR_FINISH,
    OTA_ERR_SETPARTITION,
    OTA_ERR_BADDATA,
    OTA_ERR_NODOWNGRADE,
    OTA_ERR_INVALIDFW,
    OTA_ERR_USERDECLINED,
    OTA_ERR_BADHASH,
    OTA_ERR_PATCH,
};

void handle_in_bin_data(void* ctx, uint8_t* data, size_t rawsize);

bool ota_init(jade_ota_ctx_t* joctx);
enum ota_status post_ota_check(jade_ota_ctx_t* joctx, bool* ota_end_called);
enum ota_status ota_user_validation(jade_ota_ctx_t* joctx, const uint8_t* uncompressed);
const char* ota_get_status_text(enum ota_status status);

#endif /* JADE_OTA_UTIL_H_ */
