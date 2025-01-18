export exe_name=PGA

export working_dir=${PWD}
export src_dir=src
export src_path=${working_dir}/${src_dir}

export lib_dir=lib
export lib_path=${working_dir}/${lib_dir}

export bin_dir=bin
export bin_path=${working_dir}/${bin_dir}

export build_dir=build
export build_path=${working_dir}/${build_dir}

function create_artifacts_dirs()
{
    cd ${working_dir}
    mkdir -p  ${bin_dir} ${build_dir}
}

create_artifacts_dirs


function build()
{
    create_artifacts_dirs
    cmake -B ${build_path}
    make -C ${build_path} -j
}

function run()
{
    build
    ${bin_path}/${exe_name}
}

function clean()
{
    # rm  -rf ${bin_path}/*
    # rm  -rf ${build_path}/*
    rm  -rf ${bin_path}
    rm  -rf ${build_path}
    create_artifacts_dirs
}

function feature()
{
    local message=$1
    git commit -m "[Feature]: ${message}"
}

function fix()
{
    local message=$1
    git commit -m "[Fix]: ${message}"
}


function enhancement()
{
    local message=$1
    git commit -m "[Enhancement]: ${message}"
}
