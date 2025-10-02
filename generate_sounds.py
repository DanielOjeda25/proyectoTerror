#!/usr/bin/env python3
"""
Generador de sonidos de pisadas para el juego
Crea sonidos WAV de pisadas normales y de correr
"""

import numpy as np
import wave
import os
import math

def generate_footstep_sound(duration=0.3, sample_rate=44100, frequency=200, volume=0.3):
    """Genera un sonido de pisada"""
    samples = int(duration * sample_rate)
    
    # Crear ruido blanco como base
    noise = np.random.normal(0, 0.1, samples)
    
    # Añadir tono principal
    t = np.linspace(0, duration, samples)
    tone = np.sin(2 * np.pi * frequency * t) * volume
    
    # Añadir armónicos para hacer el sonido más realista
    harmonic1 = np.sin(2 * np.pi * frequency * 2 * t) * volume * 0.3
    harmonic2 = np.sin(2 * np.pi * frequency * 3 * t) * volume * 0.1
    
    # Combinar todos los elementos
    sound = noise + tone + harmonic1 + harmonic2
    
    # Aplicar envolvente (fade in/out)
    envelope = np.ones(samples)
    fade_samples = int(0.01 * sample_rate)  # 10ms fade
    envelope[:fade_samples] = np.linspace(0, 1, fade_samples)
    envelope[-fade_samples:] = np.linspace(1, 0, fade_samples)
    
    sound = sound * envelope
    
    # Normalizar
    sound = np.clip(sound, -1, 1)
    
    return sound

def generate_running_sound(duration=0.2, sample_rate=44100, frequency=250, volume=0.4):
    """Genera un sonido de correr (más rápido y agudo)"""
    samples = int(duration * sample_rate)
    
    # Crear ruido blanco como base
    noise = np.random.normal(0, 0.15, samples)
    
    # Añadir tono principal más agudo
    t = np.linspace(0, duration, samples)
    tone = np.sin(2 * np.pi * frequency * t) * volume
    
    # Añadir armónicos
    harmonic1 = np.sin(2 * np.pi * frequency * 2 * t) * volume * 0.4
    harmonic2 = np.sin(2 * np.pi * frequency * 3 * t) * volume * 0.2
    
    # Combinar todos los elementos
    sound = noise + tone + harmonic1 + harmonic2
    
    # Aplicar envolvente más agresiva
    envelope = np.ones(samples)
    fade_samples = int(0.005 * sample_rate)  # 5ms fade (más rápido)
    envelope[:fade_samples] = np.linspace(0, 1, fade_samples)
    envelope[-fade_samples:] = np.linspace(1, 0, fade_samples)
    
    sound = sound * envelope
    
    # Normalizar
    sound = np.clip(sound, -1, 1)
    
    return sound

def save_wav(filename, sound, sample_rate=44100):
    """Guarda el sonido como archivo WAV"""
    # Convertir a 16-bit PCM
    sound_16bit = (sound * 32767).astype(np.int16)
    
    with wave.open(filename, 'w') as wav_file:
        wav_file.setnchannels(1)  # Mono
        wav_file.setsampwidth(2)  # 16-bit
        wav_file.setframerate(sample_rate)
        wav_file.writeframes(sound_16bit.tobytes())

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
    
    # Crear ruido ambiental (como viento o ambiente)
    ambient_noise = np.random.normal(0, 0.05, ambient_samples)
    
    # Añadir tonos bajos para ambiente
    t = np.linspace(0, ambient_duration, ambient_samples)
    ambient_tone1 = np.sin(2 * np.pi * 60 * t) * 0.1  # 60 Hz
    ambient_tone2 = np.sin(2 * np.pi * 120 * t) * 0.05  # 120 Hz
    
    ambient_sound = ambient_noise + ambient_tone1 + ambient_tone2
    
    # Aplicar envolvente suave
    envelope = np.ones(ambient_samples)
    fade_samples = int(0.5 * 44100)  # 0.5 segundos fade
    envelope[:fade_samples] = np.linspace(0, 1, fade_samples)
    envelope[-fade_samples:] = np.linspace(1, 0, fade_samples)
    
    ambient_sound = ambient_sound * envelope
    ambient_sound = np.clip(ambient_sound, -1, 1)
    
    save_wav("sounds/ambient.wav", ambient_sound)
    print("Generado: sounds/ambient.wav")
    
    print("\n¡Todos los sonidos generados exitosamente!")
    print("Archivos creados:")
    print("- sounds/footstep_1.wav a footstep_4.wav (pisadas normales)")
    print("- sounds/running_1.wav a running_3.wav (pisadas de correr)")
    print("- sounds/ambient.wav (sonido ambiental)")

if __name__ == "__main__":
    main()
