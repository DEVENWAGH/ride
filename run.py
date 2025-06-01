#!/usr/bin/env python3
"""
RideEasy - One-click compile and run script
Usage: python run.py
"""

import os
import sys
import subprocess
import platform
import shutil
from pathlib import Path

def print_header():
    print("=" * 50)
    print("🚗 RideEasy - Ride Sharing Platform")
    print("=" * 50)

def check_compiler():
    """Check if required compiler is available"""
    compilers = ['g++', 'clang++']
    
    for compiler in compilers:
        if shutil.which(compiler):
            print(f"✅ Found compiler: {compiler}")
            return compiler
    
    print("❌ No C++ compiler found!")
    print("Please install one of the following:")
    print("  - MinGW-w64 (Windows): https://www.mingw-w64.org/")
    print("  - GCC (Linux): sudo apt install g++")
    print("  - Xcode Command Line Tools (macOS): xcode-select --install")
    return None

def clean_build():
    """Clean previous build artifacts"""
    files_to_remove = ['rideeasy.exe', 'rideeasy', 'a.out']
    for file in files_to_remove:
        if os.path.exists(file):
            os.remove(file)
            print(f"🧹 Cleaned: {file}")

def compile_project(compiler):
    """Compile the C++ project"""
    print(f"\n🔨 Compiling with {compiler}...")
    
    # Determine executable name based on platform
    exe_name = "rideeasy.exe" if platform.system() == "Windows" else "rideeasy"
    
    # Compilation command
    cmd = [
        compiler,
        "-std=c++17",
        "-Wall",
        "-Wextra",
        "-O2",  # Optimization
        "-I.",  # Include current directory
        "main.cpp",
        "-o", exe_name
    ]
    
    # Add static linking for Windows MinGW
    if platform.system() == "Windows" and "g++" in compiler:
        cmd.extend(["-static-libgcc", "-static-libstdc++"])
    
    print(f"Command: {' '.join(cmd)}")
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
        
        if result.returncode == 0:
            print("✅ Compilation successful!")
            return exe_name
        else:
            print("❌ Compilation failed!")
            print("STDOUT:", result.stdout)
            print("STDERR:", result.stderr)
            return None
            
    except subprocess.TimeoutExpired:
        print("❌ Compilation timed out!")
        return None
    except Exception as e:
        print(f"❌ Compilation error: {e}")
        return None

def run_executable(exe_name):
    """Run the compiled executable"""
    print(f"\n🚀 Running {exe_name}...")
    print("-" * 50)
    
    try:
        if platform.system() == "Windows":
            subprocess.run([exe_name], check=True)
        else:
            subprocess.run([f"./{exe_name}"], check=True)
        
        print("-" * 50)
        print("✅ Program executed successfully!")
        
    except subprocess.CalledProcessError as e:
        print(f"❌ Runtime error (exit code {e.returncode})")
    except KeyboardInterrupt:
        print("\n⚠️ Program interrupted by user")
    except Exception as e:
        print(f"❌ Execution error: {e}")

def main():
    print_header()
    
    # Change to script directory
    script_dir = Path(__file__).parent.absolute()
    os.chdir(script_dir)
    print(f"📁 Working directory: {script_dir}")
    
    # Check for required files
    required_files = ['main.cpp', 'User.h', 'RideManager.h']
    missing_files = [f for f in required_files if not os.path.exists(f)]
    
    if missing_files:
        print(f"❌ Missing required files: {missing_files}")
        return 1
    
    # Clean previous builds
    clean_build()
    
    # Check compiler
    compiler = check_compiler()
    if not compiler:
        return 1
    
    # Compile project
    exe_name = compile_project(compiler)
    if not exe_name:
        return 1
    
    # Run executable
    run_executable(exe_name)
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
