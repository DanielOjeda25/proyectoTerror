#!/usr/bin/env python3
"""
Generador de sonidos de pisadas para el juego
Crea sonidos WAV de pisadas normales y de correr
"""

import numpy as np
import wave
import os
import math

def generate_footstep_sound(duration=0.2, sample_rate=44100, frequency=80, volume=0.4):
    """Genera un sonido de pisada estilo 8-bit arcade"""
    samples = int(duration * sample_rate)
    
    # Crear tono principal de baja frecuencia (estilo arcade)
    t = np.linspace(0, duration, samples)
    tone = np.sin(2 * np.pi * frequency * t) * volume
    
    # Añadir armónico para sonido más rico
    harmonic = np.sin(2 * np.pi * frequency * 2 * t) * volume * 0.3
    
    # Combinar elementos (sin ruido para sonido limpio)
    sound = tone + harmonic
    
    # Aplicar envolvente más agresiva (estilo arcade)
    envelope = np.ones(samples)
    attack_samples = int(0.005 * sample_rate)  # 5ms attack
    decay_samples = int(0.1 * sample_rate)     # 100ms decay
    
    # Attack
    envelope[:attack_samples] = np.linspace(0, 1, attack_samples)
    # Decay
    envelope[attack_samples:attack_samples + decay_samples] = np.linspace(1, 0.3, decay_samples)
    # Sustain
    envelope[attack_samples + decay_samples:] = 0.3
    
    sound = sound * envelope
    
    # Agregar eco sutil para soledad
    echo_delay = int(0.1 * sample_rate)  # 100ms de delay
    echo_decay = 0.2  # Decaimiento del eco
    
    if len(sound) > echo_delay:
        echo = np.zeros_like(sound)
        echo[echo_delay:] = sound[:-echo_delay] * echo_decay
        sound = sound + echo
    
    # Normalizar
    sound = np.clip(sound, -1, 1)
    
    return sound

def generate_running_sound(duration=0.15, sample_rate=44100, frequency=120, volume=0.5):
    """Genera un sonido de correr estilo 8-bit arcade"""
    samples = int(duration * sample_rate)
    
    # Crear tono principal más agudo (estilo arcade)
    t = np.linspace(0, duration, samples)
    tone = np.sin(2 * np.pi * frequency * t) * volume
    
    # Añadir armónicos para sonido más rico
    harmonic1 = np.sin(2 * np.pi * frequency * 2 * t) * volume * 0.4
    harmonic2 = np.sin(2 * np.pi * frequency * 3 * t) * volume * 0.2
    
    # Combinar elementos (sin ruido para sonido limpio)
    sound = tone + harmonic1 + harmonic2
    
    # Aplicar envolvente más agresiva (estilo arcade)
    envelope = np.ones(samples)
    attack_samples = int(0.003 * sample_rate)  # 3ms attack
    decay_samples = int(0.08 * sample_rate)    # 80ms decay
    
    # Attack
    envelope[:attack_samples] = np.linspace(0, 1, attack_samples)
    # Decay
    envelope[attack_samples:attack_samples + decay_samples] = np.linspace(1, 0.2, decay_samples)
    # Sustain
    envelope[attack_samples + decay_samples:] = 0.2
    
    sound = sound * envelope
    
    # Agregar eco sutil
    echo_delay = int(0.08 * sample_rate)  # 80ms de delay
    echo_decay = 0.15  # Decaimiento del eco
    
    if len(sound) > echo_delay:
        echo = np.zeros_like(sound)
        echo[echo_delay:] = sound[:-echo_delay] * echo_decay
        sound = sound + echo
    
    # Normalizar
    sound = np.clip(sound, -1, 1)
    
    return sound

def save_wav(filename, sound, sample_rate=44100):
    """Guarda el sonido como archivo WAV de 8 bits"""
    # Convertir a 8-bit PCM
    sound_8bit = (sound * 127 + 128).astype(np.uint8)
    
    with wave.open(filename, 'w') as wav_file:
        wav_file.setnchannels(1)  # Mono
        wav_file.setsampwidth(1)  # 8-bit
        wav_file.setframerate(sample_rate)
        wav_file.writeframes(sound_8bit.tobytes())

def main():
    """Función principal para generar todos los sonidos"""
    print("Generando sonidos de pisadas...")
    
    # Crear directorio de sonidos si no existe
    os.makedirs("sounds", exist_ok=True)
    
    # Generar sonidos de pisadas normales (variaciones)
    for i in range(4):
        # Variar ligeramente la frecuencia y duración
        freq = 180 + i * 20
        dur = 0.3 + i * 0.05
        vol = 0.3 + i * 0.05
        
        sound = generate_footstep_sound(duration=dur, frequency=freq, volume=vol)
        filename = f"sounds/footstep_{i+1}.wav"
        save_wav(filename, sound)
        print(f"Generado: {filename}")
    
    # Generar sonidos de correr (variaciones)
    for i in range(3):
        # Variar ligeramente la frecuencia y duración
        freq = 220 + i * 30
        dur = 0.2 + i * 0.03
        vol = 0.4 + i * 0.05
        
        sound = generate_running_sound(duration=dur, frequency=freq, volume=vol)
        filename = f"sounds/running_{i+1}.wav"
        save_wav(filename, sound)
        print(f"Generado: {filename}")
    
    # Generar sonido ambiental
    print("Generando sonido ambiental...")
    ambient_duration = 10.0  # 10 segundos
    ambient_samples = int(ambient_duration * 44100)
    
    # Crear ruido ambiental muy suave (como viento o ambiente)
    ambient_noise = np.random.normal(0, 0.02, ambient_samples)  # Reducido de 0.05 a 0.02
    
    # Añadir tonos bajos muy suaves para ambiente
    t = np.linspace(0, ambient_duration, ambient_samples)
    ambient_tone1 = np.sin(2 * np.pi * 60 * t) * 0.03  # Reducido de 0.1 a 0.03
    ambient_tone2 = np.sin(2 * np.pi * 120 * t) * 0.015  # Reducido de 0.05 a 0.015
    
    ambient_sound = ambient_noise + ambient_tone1 + ambient_tone2
    
    # Aplicar envolvente suave
    envelope = np.ones(ambient_samples)
    fade_samples = int(0.5 * 44100)  # 0.5 segundos fade
    envelope[:fade_samples] = np.linspace(0, 1, fade_samples)
    envelope[-fade_samples:] = np.linspace(1, 0, fade_samples)
    
    ambient_sound = ambient_sound * envelope
    ambient_sound = np.clip(ambient_sound, -1, 1) * 0.4  # Reducir volumen general a 40%
    
    save_wav("sounds/ambient.wav", ambient_sound)
    print("Generado: sounds/ambient.wav")
    
    print("\n¡Todos los sonidos generados exitosamente!")
    print("Archivos creados:")
    print("- sounds/footstep_1.wav a footstep_4.wav (pisadas normales)")
    print("- sounds/running_1.wav a running_3.wav (pisadas de correr)")
    print("- sounds/ambient.wav (sonido ambiental)")

if __name__ == "__main__":
    main()
