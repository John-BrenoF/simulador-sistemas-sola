import subprocess
import sys
import os
import platform

def run_command(cmd, check=True, capture_output=False):
    try:
        result = subprocess.run(cmd, shell=True, check=check, capture_output=capture_output)
        if capture_output:
            return result.stdout.decode('utf-8').strip()
        return True
    except subprocess.CalledProcessError as e:
        print(f"Command failed: {cmd}\nError: {e}")
        if capture_output:
            print(f"Output: {e.output.decode('utf-8') if e.output else 'No output'}")
        return False

def get_distro():
    try:
        with open('/etc/os-release', 'r') as f:
            lines = f.readlines()
            for line in lines:
                if line.startswith('ID='):
                    return line.strip().split('=')[1].strip('"')
    except:
        pass
    return None

def install_with_apt(packages):
    print("Updating apt...")
    if not run_command('sudo apt update'):
        return False
    print(f"Installing packages: {', '.join(packages)}")
    return run_command(f'sudo apt install -y {" ".join(packages)}')

def install_with_yum(packages):
    print(f"Installing packages: {', '.join(packages)}")
    return run_command(f'sudo yum install -y {" ".join(packages)}')

def install_with_dnf(packages):
    print(f"Installing packages: {', '.join(packages)}")
    return run_command(f'sudo dnf install -y {" ".join(packages)}')

def check_package_apt(package):
    output = run_command(f'dpkg -l {package}', capture_output=True)
    return package in output

def check_package_yum(package):
    output = run_command(f'rpm -q {package}', capture_output=True)
    return 'is not installed' not in output

def check_package_dnf(package):
    output = run_command(f'rpm -q {package}', capture_output=True)
    return 'is not installed' not in output

def install_dependencies():
    distro = get_distro()
    print(f"Detected distro: {distro}")

    packages = {
        'build-essential': ['g++'],
        'freeglut3-dev': ['freeglut3-dev', 'libgl1-mesa-dev', 'libglu1-mesa-dev'],
        'mesa-utils': ['mesa-utils']
    }

    success = False
    if distro in ['ubuntu', 'debian']:
        for pkg_group, apt_pkgs in packages.items():
            for pkg in apt_pkgs:
                if not check_package_apt(pkg):
                    print(f"{pkg} not found, installing...")
                    if not install_with_apt(apt_pkgs):
                        print("Apt installation failed.")
                        success = False
                        break
            success = True
    elif distro in ['fedora', 'rhel']:
        for pkg_group, yum_pkgs in packages.items():
            yum_pkgs = [p.replace('-dev', '-devel') for p in yum_pkgs] 
            yum_pkgs = list(set(yum_pkgs + ['gcc-c++', 'freeglut-devel', 'mesa-libGL-devel', 'mesa-libGLU-devel']))
            for pkg in yum_pkgs:
                if not check_package_dnf(pkg):
                    print(f"{pkg} not found, installing...")
                    if not install_with_dnf(yum_pkgs):
                        print("DNF installation failed.")
                        success = False
                        break
            success = True
    elif distro in ['centos']:
        for pkg_group, yum_pkgs in packages.items():
            yum_pkgs = [p.replace('-dev', '-devel') for p in yum_pkgs]
            yum_pkgs = list(set(yum_pkgs + ['gcc-c++', 'freeglut-devel', 'mesa-libGL-devel', 'mesa-libGLU-devel']))
            for pkg in yum_pkgs:
                if not check_package_yum(pkg):
                    print(f"{pkg} not found, installing...")
                    if not install_with_yum(yum_pkgs):
                        print("Yum installation failed.")
                        success = False
                        break
            success = True

    if not success:
        print("Primary package manager failed. Trying alternative (yum/dnf)...")
        fallback_pkgs = ['gcc-c++', 'freeglut-devel', 'mesa-libGL-devel', 'mesa-libGLU-devel']
        if run_command('sudo dnf --version', check=False):
            install_with_dnf(fallback_pkgs)
        elif run_command('sudo yum --version', check=False):
            install_with_yum(fallback_pkgs)
        else:
            print("No suitable package manager found. Please install dependencies manually: g++, freeglut3-dev, libgl1-mesa-dev, libglu1-mesa-dev")
            return False

    if not run_command('g++ --version', check=False):
        print("g++ not found after installation. Manual intervention needed.")
        return False

    return True

def compile_code():
    if not os.path.exists('main.cpp'):
        print("main.cpp not found in current directory.")
        return False

    print("Compiling main.cpp...")
    cmd = 'g++ -o simulator main.cpp -lGL -lGLU -lglut -std=c++11'
    if run_command(cmd):
        print("Compilation successful.")
        return True
    else:
        print("Compilation failed. Check errors above.")
        return False

def run_simulator():
    if os.path.exists('simulator'):
        print("Running simulator...")
        run_command('./simulator')
        return True
    else:
        print("Executable not found.")
        return False

def main():
    print("Planet Simulator Setup Script")
    print("=============================")

    if install_dependencies():
        print("Dependencies installed/verified.")
        if compile_code():
            print("Ready to run.")
            run_simulator()
        else:
            print("Compilation failed. Run 'g++ -o simulator main.cpp -lGL -lGLU -lglut -std=c++11' manually.")
    else:
        print("Dependency installation failed. Please install manually and rerun.")

if __name__ == "__main__":
    main()