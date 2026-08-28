# ビルドコマンド例
# cmake -B CmakeBuilds -G Xcode
# cmake --build CmakeBuilds --config Release
# or
# cmake -B CmakeBuilds -G "Visual Studio 17 2022"
# cmake --build CmakeBuilds --config Release
# or
# cmake -B CmakeBuilds -G "Ninja Multi-Config"
# cmake --build CmakeBuilds --config Release
# or
# cmake -B CmakeBuilds && cd CmakeBuilds && make
# 
# ビルドコマンド例 - CMake Presets 機能を使う場合
# cmake --preset xcode
# cmake --build --preset xcode-release
# or
# cmake --preset vs2022
# cmake --build --preset vs2022-release
# or
# cmake --preset ninja-multi
# cmake --build --preset ninja-release

# サポート対象とするCMakeの最小バージョンを3.22とする
cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

# MusicPlayerプロジェクトを追加する
project(MusicPlayer VERSION 1.0.0)

# CMAKE_EXPORT_COMPILE_COMMANDSフラグを有効にすることでVisual Studio Code等で補完処理が効くようにする
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# ../../lib/JUCEにJUCEがインストールされていることを確認する
if(EXISTS ../../lib/JUCE AND IS_DIRECTORY ../../lib/JUCE)
    # ../../lib/JUCEがインストールされている場合はパス指定でJUCEをインクルードする
    add_subdirectory(../../lib/JUCE lib/JUCE)
else()
    # ../../lib/JUCEがインストールされていない場合はJUCEをインストールするディレクトリを読者に案内する
    message(FATAL_ERROR "Please install JUCE in ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/JUCE directory.")
endif()

# MSVCにおいてソースファイルをマルチバイト文字で記述する際にUTF-8としてコンパイラに認識させる
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/utf-8)
endif()

# JUCE GUI Application ターゲットを追加します
# バージョン番号を含む各種プロパティを設定します
juce_add_gui_app(MusicPlayer
    VERSION 1.0.0                       # Set this if the app version is different to the project version
    # ICON_BIG ...                      # ICON_* arguments specify a path to an image file to use as an icon
    # ICON_SMALL ...
    # DOCUMENT_EXTENSIONS ...           # Specify file extensions that should be associated with this app
    # COMPANY_NAME ...                  # Specify the name of the app's author
    PRODUCT_NAME "MusicPlayer")     # The name of the final executable, which can differ from the target name

# JuceHeader.hを作成しビルドツリーに生成します
# コード内で#include <JuceHeader.h>とインクルードしている場合に使用します
juce_generate_juce_header(MusicPlayer)

# 標準CMakeコマンド: ソースファイルをターゲットに追加します
# GLOB_RECURSE      ...フォルダ内を再帰的にリストアップするGLOB
# CONFIGURE_DEPENDS ...指定することで,ファイルリストが変化するとMakefileを再生成する
file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS ./Source/*.cpp ./Source/*.h)
target_sources(MusicPlayer
    PRIVATE
        ${SourceFiles})

# 標準CMakeコマンド: ターゲットのコンパイル時に使用するマクロ定義など
# Projucer同様Preprocessor Definitionsをターゲットに追加
# 特定の機能のON/OFF切り替えが可能
target_compile_definitions(MusicPlayer
    PRIVATE
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        DONT_SET_USING_JUCE_NAMESPACE=1         # `using namespace juce`の宣言を無効化する
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:MusicPlayer,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:MusicPlayer,JUCE_VERSION>")

# JUCEモジュールなどを実行可能ファイルにリンク
# JUCEモジュールの場合は依存している他のJUCEモジュールも自動的に含まれる
target_link_libraries(MusicPlayer
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
