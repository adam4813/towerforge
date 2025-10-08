#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>
#include <memory>

namespace towerforge {
namespace audio {

/**
 * @brief Types of audio tracks
 */
enum class AudioType {
    Music,
    SFX,
    Ambient
};

/**
 * @brief Audio cue identifiers for consistent playback
 */
enum class AudioCue {
    // Menu sounds
    MenuClick,
    MenuConfirm,
    MenuError,
    MenuOpen,
    MenuClose,
    
    // Gameplay sounds
    FacilityPlace,
    FacilityDemolish,
    ElevatorDing,
    ElevatorChime,
    Construction,
    ConstructionComplete,
    
    // Event sounds
    Achievement,
    Milestone,
    Toast,
    Alert,
    
    // Error sounds
    InvalidPlacement,
    ErrorBuzz,
    
    // Music tracks
    MainTheme,
    GameplayLoop,
    VictoryTheme,
    Fanfare
};

/**
 * @brief Centralized audio manager for TowerForge
 * 
 * Manages all audio playback including background music, sound effects,
 * and ambient audio. Supports volume control, fading, and graceful
 * error handling for missing or corrupt files.
 * 
 * This is a singleton class - use GetInstance() to access it.
 */
class AudioManager {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the AudioManager instance
     */
    static AudioManager& GetInstance();
    
    /**
     * @brief Initialize the audio system
     * 
     * Must be called before any audio operations.
     * Initializes the audio device and loads initial audio resources.
     */
    void Initialize();
    
    /**
     * @brief Shutdown the audio system
     * 
     * Unloads all audio resources and closes the audio device.
     * Called automatically on destruction.
     */
    void Shutdown();
    
    /**
     * @brief Update audio system (called every frame)
     * @param delta_time Time elapsed since last frame
     */
    void Update(float delta_time);
    
    /**
     * @brief Play a music track
     * @param cue Music cue to play
     * @param loop Whether to loop the music
     * @param fade_in_duration Duration to fade in (seconds), 0 for instant
     */
    void PlayMusic(AudioCue cue, bool loop = true, float fade_in_duration = 0.0f);
    
    /**
     * @brief Stop currently playing music
     * @param fade_out_duration Duration to fade out (seconds), 0 for instant
     */
    void StopMusic(float fade_out_duration = 0.0f);
    
    /**
     * @brief Play a sound effect
     * @param cue SFX cue to play
     * @param volume Volume multiplier (0.0 to 1.0), -1 for default
     */
    void PlaySFX(AudioCue cue, float volume = -1.0f);
    
    /**
     * @brief Play an ambient sound
     * @param cue Ambient cue to play
     * @param loop Whether to loop the ambient sound
     * @param fade_in_duration Duration to fade in (seconds), 0 for instant
     */
    void PlayAmbient(AudioCue cue, bool loop = true, float fade_in_duration = 0.0f);
    
    /**
     * @brief Stop currently playing ambient sound
     * @param fade_out_duration Duration to fade out (seconds), 0 for instant
     */
    void StopAmbient(float fade_out_duration = 0.0f);
    
    /**
     * @brief Set volume for a specific audio type
     * @param type Audio type (Music, SFX, or Ambient)
     * @param volume Volume level (0.0 to 1.0)
     */
    void SetVolume(AudioType type, float volume);
    
    /**
     * @brief Get current volume for a specific audio type
     * @param type Audio type (Music, SFX, or Ambient)
     * @return Current volume level (0.0 to 1.0)
     */
    float GetVolume(AudioType type) const;
    
    /**
     * @brief Set master volume
     * @param volume Master volume level (0.0 to 1.0)
     */
    void SetMasterVolume(float volume);
    
    /**
     * @brief Get master volume
     * @return Master volume level (0.0 to 1.0)
     */
    float GetMasterVolume() const;
    
    /**
     * @brief Check if audio system is initialized
     * @return True if initialized
     */
    bool IsInitialized() const { return initialized_; }
    
    // Delete copy constructor and assignment operator (singleton)
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;
    
private:
    AudioManager();
    ~AudioManager();
    
    /**
     * @brief Load an audio file with error handling
     * @param cue Audio cue identifier
     * @param filepath Path to audio file
     * @param is_music True if this is a music track
     * @return True if loaded successfully
     */
    bool LoadAudioFile(AudioCue cue, const std::string& filepath, bool is_music);
    
    /**
     * @brief Get the file path for an audio cue
     * @param cue Audio cue identifier
     * @return File path string, or empty if not found
     */
    std::string GetAudioFilePath(AudioCue cue) const;
    
    /**
     * @brief Update music fading
     * @param delta_time Time elapsed since last frame
     */
    void UpdateMusicFade(float delta_time);
    
    /**
     * @brief Update ambient fading
     * @param delta_time Time elapsed since last frame
     */
    void UpdateAmbientFade(float delta_time);
    
    bool initialized_;
    
    // Volume settings
    float master_volume_;
    float music_volume_;
    float sfx_volume_;
    float ambient_volume_;
    
    // Music management
    Music current_music_;
    bool music_loaded_;
    float music_fade_target_;
    float music_fade_rate_;
    bool music_fading_;
    bool music_stopping_;
    float current_music_volume_;
    
    // Ambient management
    Music current_ambient_;
    bool ambient_loaded_;
    float ambient_fade_target_;
    float ambient_fade_rate_;
    bool ambient_fading_;
    bool ambient_stopping_;
    float current_ambient_volume_;
    
    // Audio resource maps
    std::unordered_map<AudioCue, Music> music_cache_;
    std::unordered_map<AudioCue, Sound> sfx_cache_;
    
    // Track which audio files failed to load to avoid repeated warnings
    std::unordered_map<AudioCue, bool> load_failures_;
};

} // namespace audio
} // namespace towerforge
