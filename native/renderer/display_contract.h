#pragma once

// Owned exclusively by the standalone APK. Never fall back to another app's
// controls/status or treat its loaded renderer as ours.
namespace seed_lens {
inline constexpr char seed_control_leaf[]="seed-lens-seed-preview.txt";
inline constexpr char seed_control[]="/data/local/tmp/seed-lens-seed-preview.txt";
inline constexpr char seed_report_leaf[]="seed_lens_seed_preview_status.tsv";
inline constexpr char seed_report[]="/data/user/0/com.nianticlabs.pikmin/files/seed_lens_seed_preview_status.tsv";
inline constexpr char payload_lock[]="/data/user/0/com.nianticlabs.pikmin/files/seed_lens_payload.lock";
}
