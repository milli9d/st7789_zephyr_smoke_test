#!/usr/bin/env bash
install_dir=$1
zephyr_tag=v4.0.0

# navigate to home directory
if [[ -z "${install_dir}" ]]; then
    install_dir=~/zephyrproject
fi

echo -e "Installing ZephyrSDK"
echo -e "Path: ${install_dir}"
echo -e "============================================================"

# install brew
if [[ -z "$(command -v brew)" ]]; then
    (
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        (
            echo
            echo 'eval "$(/opt/homebrew/bin/brew shellenv)"'
        ) >>~/.zprofile
        source ~/.zprofile
        brew install cmake ninja gperf python3 python-tk ccache qemu dtc libmagic wget openocd
        (
            echo
            echo 'export PATH="'$(brew --prefix)'/opt/python/libexec/bin:$PATH"'
        ) >>~/.zprofile
        source ~/.zprofile
    )
else
    echo "Brew is already installed! Skipping."
fi

# make venv
venv_path=${install_dir}/.venv
if [[ -e "${venv_path}" ]]; then
    echo "Using existing venv at ${venv_path}"
else
    (
        echo "Making a new venv at ${venv_path}"
        python3 -m venv ~/zephyrproject/.venv
    )
fi

# activate venv in parent shell; any subshells after this should have access
source ~/zephyrproject/.venv/bin/activate

# install west
if $(command -v west)
then
    echo -e "Using existing west installation at $(command -v west)"
else
    (
        pip install west
        west init ~/zephyrproject
    )
fi

# Run a west update and update python deps
(
    cd ~/zephyrproject
    west update
    west zephyr-export
    pip install -r ~/zephyrproject/zephyr/scripts/requirements.txt
)

# install zephyr SDK
zephyr_sdk_dir=${install_dir}/zephyr
if [[ -e ${zephyr_sdk_dir} ]]; then
    echo "Zephyr SDK already installed!"
    exit 0
else
    # install
    (
        cd ~/zephyrproject/zephyr
        west sdk install
    )
fi

(
        cd ~/zephyrproject/zephyr
        git checkout ${zephyr_tag}
        west update
)