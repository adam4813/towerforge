#include "audio/audio_manager.h"
#include <iostream>
#include <cmath>

namespace towerforge::audio {

    // Singleton instance
    AudioManager& AudioManager::GetInstance() {
        static AudioManager instance;
        return instance;
    }

    AudioManager::AudioManager()
        : initialized_(false)
          , master_volume_(0.7f)
          , music_volume_(0.5f)
          , sfx_volume_(0.6f)
          , ambient_volume_(0.4f)
          , music_loaded_(false)
          , music_fade_target_(1.0f)
          , music_fade_rate_(0.0f)
          , music_fading_(false)
          , music_stopping_(false)
          , current_music_volume_(0.0f)
          , ambient_loaded_(false)
          , ambient_fade_target_(1.0f)
          , ambient_fade_rate_(0.0f)
          , ambient_fading_(false)
          , ambient_stopping_(false)
          , current_ambient_volume_(0.0f) {
    }

    AudioManager::~AudioManager() {
        Shutdown();
    }

    void AudioManager::Initialize() {
        if (initialized_) {
            return;
        }
    
        std::cout << "Initializing audio system..." << std::endl;
    
        // Initialize audio device
        InitAudioDevice();
    
        if (!IsAudioDeviceReady()) {
            std::cerr << "Failed to initialize audio device!" << std::endl;
            return;
        }
    
        initialized_ = true;
        std::cout << "Audio system initialized successfully" << std::endl;
    }

    void AudioManager::Shutdown() {
        if (!initialized_) {
            return;
        }
    
        std::cout << "Shutting down audio system..." << std::endl;
    
        // Stop all playing audio
        StopMusic(0.0f);
        StopAmbient(0.0f);
    
        // Unload all cached music
        for (const auto& pair : music_cache_) {
            UnloadMusicStream(pair.second);
        }
        music_cache_.clear();
    
        // Unload all cached sound effects
        for (const auto& pair : sfx_cache_) {
            UnloadSound(pair.second);
        }
        sfx_cache_.clear();
    
        // Close audio device
        CloseAudioDevice();
    
        initialized_ = false;
        std::cout << "Audio system shut down" << std::endl;
    }

    void AudioManager::Update(const float delta_time) {
        if (!initialized_) {
            return;
        }
    
        // Update music stream
        if (music_loaded_ && IsMusicStreamPlaying(current_music_)) {
            UpdateMusicStream(current_music_);
        }
    
        // Update ambient stream
        if (ambient_loaded_ && IsMusicStreamPlaying(current_ambient_)) {
            UpdateMusicStream(current_ambient_);
        }
    
        // Update music fading
        if (music_fading_) {
            UpdateMusicFade(delta_time);
        }
    
        // Update ambient fading
        if (ambient_fading_) {
            UpdateAmbientFade(delta_time);
        }
    }

    void AudioManager::PlayMusic(const AudioCue cue, const bool loop, const float fade_in_duration) {
        if (!initialized_) {
            return;
        }
    
        // Stop current music if playing
        if (music_loaded_) {
            StopMusic(0.0f);
        }
    
        // Try to get from cache or load
        const Music* music_ptr = nullptr;
        const auto it = music_cache_.find(cue);
    
        if (it != music_cache_.end()) {
            music_ptr = &it->second;
        } else {
            // Try to load the music file
            const std::string filepath = GetAudioFilePath(cue);
            if (filepath.empty()) {
                if (load_failures_.find(cue) == load_failures_.end()) {
                    std::cerr << "Warning: No file path configured for music cue" << std::endl;
                    load_failures_[cue] = true;
                }
                return;
            }
        
            if (LoadAudioFile(cue, filepath, true)) {
                music_ptr = &music_cache_[cue];
            } else {
                return;
            }
        }
    
        if (music_ptr) {
            current_music_ = *music_ptr;
            music_loaded_ = true;
        
            // Set looping
            current_music_.looping = loop;
        
            // Start playing
            PlayMusicStream(current_music_);
        
            // Setup fade-in if requested
            if (fade_in_duration > 0.0f) {
                current_music_volume_ = 0.0f;
                SetMusicVolume(current_music_, 0.0f);
                music_fade_target_ = music_volume_;
                music_fade_rate_ = music_volume_ / fade_in_duration;
                music_fading_ = true;
                music_stopping_ = false;
            } else {
                current_music_volume_ = master_volume_ * music_volume_;
                SetMusicVolume(current_music_, current_music_volume_);
                music_fading_ = false;
            }
        }
    }

    void AudioManager::StopMusic(const float fade_out_duration) {
        if (!initialized_ || !music_loaded_) {
            return;
        }
    
        if (fade_out_duration > 0.0f) {
            // Fade out
            music_fade_target_ = 0.0f;
            music_fade_rate_ = music_volume_ / fade_out_duration;
            music_fading_ = true;
            music_stopping_ = true;
        } else {
            // Stop immediately
            StopMusicStream(current_music_);
            music_loaded_ = false;
            music_fading_ = false;
            music_stopping_ = false;
        }
    }

    void AudioManager::PlaySFX(const AudioCue cue, const float volume) {
        if (!initialized_) {
            return;
        }
    
        // Try to get from cache or load
        const Sound* sound_ptr = nullptr;
        const auto it = sfx_cache_.find(cue);
    
        if (it != sfx_cache_.end()) {
            sound_ptr = &it->second;
        } else {
            // Try to load the sound file
            const std::string filepath = GetAudioFilePath(cue);
            if (filepath.empty()) {
                if (load_failures_.find(cue) == load_failures_.end()) {
                    std::cerr << "Warning: No file path configured for SFX cue" << std::endl;
                    load_failures_[cue] = true;
                }
                return;
            }
        
            if (LoadAudioFile(cue, filepath, false)) {
                sound_ptr = &sfx_cache_[cue];
            } else {
                return;
            }
        }
    
        if (sound_ptr) {
            // Calculate final volume
            float final_volume = master_volume_ * sfx_volume_;
            if (volume >= 0.0f) {
                final_volume *= volume;
            }
        
            SetSoundVolume(*sound_ptr, final_volume);
            PlaySound(*sound_ptr);
        }
    }

    void AudioManager::PlayAmbient(const AudioCue cue, const bool loop, const float fade_in_duration) {
        if (!initialized_) {
            return;
        }
    
        // Stop current ambient if playing
        if (ambient_loaded_) {
            StopAmbient(0.0f);
        }
    
        // Try to get from cache or load
        const Music* music_ptr = nullptr;
        const auto it = music_cache_.find(cue);
    
        if (it != music_cache_.end()) {
            music_ptr = &it->second;
        } else {
            // Try to load the music file
            const std::string filepath = GetAudioFilePath(cue);
            if (filepath.empty()) {
                if (load_failures_.find(cue) == load_failures_.end()) {
                    std::cerr << "Warning: No file path configured for ambient cue" << std::endl;
                    load_failures_[cue] = true;
                }
                return;
            }
        
            if (LoadAudioFile(cue, filepath, true)) {
                music_ptr = &music_cache_[cue];
            } else {
                return;
            }
        }
    
        if (music_ptr) {
            current_ambient_ = *music_ptr;
            ambient_loaded_ = true;
        
            // Set looping
            current_ambient_.looping = loop;
        
            // Start playing
            PlayMusicStream(current_ambient_);
        
            // Setup fade-in if requested
            if (fade_in_duration > 0.0f) {
                current_ambient_volume_ = 0.0f;
                SetMusicVolume(current_ambient_, 0.0f);
                ambient_fade_target_ = ambient_volume_;
                ambient_fade_rate_ = ambient_volume_ / fade_in_duration;
                ambient_fading_ = true;
                ambient_stopping_ = false;
            } else {
                current_ambient_volume_ = master_volume_ * ambient_volume_;
                SetMusicVolume(current_ambient_, current_ambient_volume_);
                ambient_fading_ = false;
            }
        }
    }

    void AudioManager::StopAmbient(const float fade_out_duration) {
        if (!initialized_ || !ambient_loaded_) {
            return;
        }
    
        if (fade_out_duration > 0.0f) {
            // Fade out
            ambient_fade_target_ = 0.0f;
            ambient_fade_rate_ = ambient_volume_ / fade_out_duration;
            ambient_fading_ = true;
            ambient_stopping_ = true;
        } else {
            // Stop immediately
            StopMusicStream(current_ambient_);
            ambient_loaded_ = false;
            ambient_fading_ = false;
            ambient_stopping_ = false;
        }
    }

    void AudioManager::SetVolume(const AudioType type, float volume) {
        // Clamp volume to valid range
        volume = std::max(0.0f, std::min(1.0f, volume));
    
        switch (type) {
            case AudioType::Music:
                music_volume_ = volume;
                if (music_loaded_ && !music_fading_) {
                    current_music_volume_ = master_volume_ * music_volume_;
                    SetMusicVolume(current_music_, current_music_volume_);
                }
                break;
            
            case AudioType::SFX:
                sfx_volume_ = volume;
                break;
            
            case AudioType::Ambient:
                ambient_volume_ = volume;
                if (ambient_loaded_ && !ambient_fading_) {
                    current_ambient_volume_ = master_volume_ * ambient_volume_;
                    SetMusicVolume(current_ambient_, current_ambient_volume_);
                }
                break;
        }
    }

    float AudioManager::GetVolume(const AudioType type) const {
        switch (type) {
            case AudioType::Music:
                return music_volume_;
            case AudioType::SFX:
                return sfx_volume_;
            case AudioType::Ambient:
                return ambient_volume_;
            default:
                return 0.0f;
        }
    }

    void AudioManager::SetMasterVolume(const float volume) {
        master_volume_ = std::max(0.0f, std::min(1.0f, volume));
    
        // Update all active audio
        if (music_loaded_ && !music_fading_) {
            current_music_volume_ = master_volume_ * music_volume_;
            SetMusicVolume(current_music_, current_music_volume_);
        }
        if (ambient_loaded_ && !ambient_fading_) {
            current_ambient_volume_ = master_volume_ * ambient_volume_;
            SetMusicVolume(current_ambient_, current_ambient_volume_);
        }
    }

    float AudioManager::GetMasterVolume() const {
        return master_volume_;
    }

    bool AudioManager::LoadAudioFile(const AudioCue cue, const std::string& filepath, const bool is_music) {
        // Check if already failed to load
        if (load_failures_.find(cue) != load_failures_.end()) {
            return false;
        }
    
        try {
            if (is_music) {
                const Music music = LoadMusicStream(filepath.c_str());
                if (music.stream.buffer == nullptr) {
                    std::cerr << "Warning: Failed to load music file: " << filepath << std::endl;
                    load_failures_[cue] = true;
                    return false;
                }
                music_cache_[cue] = music;
            } else {
                const Sound sound = LoadSound(filepath.c_str());
                if (sound.stream.buffer == nullptr) {
                    std::cerr << "Warning: Failed to load sound file: " << filepath << std::endl;
                    load_failures_[cue] = true;
                    return false;
                }
                sfx_cache_[cue] = sound;
            }
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Exception loading audio file " << filepath << ": " << e.what() << std::endl;
            load_failures_[cue] = true;
            return false;
        }
    }

    std::string AudioManager::GetAudioFilePath(const AudioCue cue) {
        // Map audio cues to file paths
        // Note: Files don't exist yet, so these will fail gracefully
        // In a production environment, these would point to actual audio files
    
        switch (cue) {
            // Menu sounds
            case AudioCue::MenuClick:
                return "assets/audio/sfx/menu_click.wav";
            case AudioCue::MenuConfirm:
                return "assets/audio/sfx/menu_confirm.wav";
            case AudioCue::MenuError:
                return "assets/audio/sfx/menu_error.wav";
            case AudioCue::MenuOpen:
                return "assets/audio/sfx/menu_open.wav";
            case AudioCue::MenuClose:
                return "assets/audio/sfx/menu_close.wav";
            
            // Gameplay sounds
            case AudioCue::FacilityPlace:
                return "assets/audio/sfx/facility_place.wav";
            case AudioCue::FacilityDemolish:
                return "assets/audio/sfx/facility_demolish.wav";
            case AudioCue::ElevatorDing:
                return "assets/audio/sfx/elevator_ding.wav";
            case AudioCue::ElevatorChime:
                return "assets/audio/sfx/elevator_chime.wav";
            case AudioCue::Construction:
                return "assets/audio/sfx/construction.wav";
            case AudioCue::ConstructionComplete:
                return "assets/audio/sfx/construction_complete.wav";
            
            // Event sounds
            case AudioCue::Achievement:
                return "assets/audio/sfx/achievement.wav";
            case AudioCue::Milestone:
                return "assets/audio/sfx/milestone.wav";
            case AudioCue::Toast:
                return "assets/audio/sfx/toast.wav";
            case AudioCue::Alert:
                return "assets/audio/sfx/alert.wav";
            
            // Error sounds
            case AudioCue::InvalidPlacement:
                return "assets/audio/sfx/invalid_placement.wav";
            case AudioCue::ErrorBuzz:
                return "assets/audio/sfx/error_buzz.wav";
            
            // Music tracks
            case AudioCue::MainTheme:
                return "assets/audio/music/main_theme.ogg";
            case AudioCue::GameplayLoop:
                return "assets/audio/music/gameplay_loop.ogg";
            case AudioCue::VictoryTheme:
                return "assets/audio/music/victory_theme.ogg";
            case AudioCue::Fanfare:
                return "assets/audio/music/fanfare.ogg";
            
            default:
                return "";
        }
    }

    void AudioManager::UpdateMusicFade(const float delta_time) {
        if (!music_loaded_) {
            music_fading_ = false;
            return;
        }
    
        // Calculate target volume with master
        const float target_vol = master_volume_ * music_fade_target_;
    
        // Apply fade
        if (music_stopping_) {
            // Fading out
            current_music_volume_ -= music_fade_rate_ * delta_time;
            if (current_music_volume_ <= 0.0f) {
                current_music_volume_ = 0.0f;
                music_fading_ = false;
                StopMusicStream(current_music_);
                music_loaded_ = false;
                music_stopping_ = false;
            }
        } else {
            // Fading in
            current_music_volume_ += music_fade_rate_ * delta_time;
            if (current_music_volume_ >= target_vol) {
                current_music_volume_ = target_vol;
                music_fading_ = false;
            }
        }
    
        SetMusicVolume(current_music_, current_music_volume_);
    }

    void AudioManager::UpdateAmbientFade(const float delta_time) {
        if (!ambient_loaded_) {
            ambient_fading_ = false;
            return;
        }
    
        // Calculate target volume with master
        const float target_vol = master_volume_ * ambient_fade_target_;
    
        // Apply fade
        if (ambient_stopping_) {
            // Fading out
            current_ambient_volume_ -= ambient_fade_rate_ * delta_time;
            if (current_ambient_volume_ <= 0.0f) {
                current_ambient_volume_ = 0.0f;
                ambient_fading_ = false;
                StopMusicStream(current_ambient_);
                ambient_loaded_ = false;
                ambient_stopping_ = false;
            }
        } else {
            // Fading in
            current_ambient_volume_ += ambient_fade_rate_ * delta_time;
            if (current_ambient_volume_ >= target_vol) {
                current_ambient_volume_ = target_vol;
                ambient_fading_ = false;
            }
        }
    
        SetMusicVolume(current_ambient_, current_ambient_volume_);
    }

}
