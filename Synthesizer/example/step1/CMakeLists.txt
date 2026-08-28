# macでは「-G Xcode」推奨
#
# windowsでは、Projucerの設定とは異なり、Visual Studioの場合でもPlugin Copy StepをEnabledにしています。
# そのため管理者権限でコマンドラインからビルドするか、管理者権限でVisual Studioを実行してからビルドをする必要があります。
#
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

# DemoSynthesiserPluginプロジェクトを追加する
project(DemoSynthesiserPlugin VERSION 1.0.0)

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

# 静的ライブラリターゲットの追加
# フォーマットなど各種プロパティを設定します
juce_add_plugin(DemoSynthesiser
    VERSION 0.0.1                               # バージョン
    ICON_BIG ./Resource/ICON_BIG.png            # スタンドアロン版のアイコンを指定します。ICON_BIG,ICON_SMALLのどちらかのみ指定することで自動でサイズ調整されたものが適用される。
    # ICON_SMALL ...
    COMPANY_NAME yourcompany                    # 作成者名
    IS_SYNTH TRUE                               # シンセサイザーか(もしくはエフェクトかどうか)
    NEEDS_MIDI_INPUT TRUE                       # MIDI入力が必要かどうか
    NEEDS_MIDI_OUTPUT FALSE                     # MIDI出力が必要かどうか
    IS_MIDI_EFFECT FALSE                        # MIDIエフェクトであるかどうか
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE           # プラグインがキーボードフォーカスを必要とするかどうか。FALSEの場合はDAW側にキーボードがハンドリングされる
    COPY_PLUGIN_AFTER_BUILD TRUE                # ビルド後にプラグインをデフォルト位置に配置するかどうか
    PLUGIN_MANUFACTURER_CODE Manu               # 少なくとも1つの大文字を含む4文字のメーカーID
    PLUGIN_CODE Xamb                            # 少なくとも1つの大文字を含む4文字のプラグインID。GarageBand10.3では最初の文字は大文字で残りの文字は小文字である必要があります。
    FORMATS AU VST3 Standalone                  # フォーマット
    PRODUCT_NAME "DemoSynthesiser")             # 実行ファイル名

# JuceHeader.hを作成しビルドツリーに生成します
# コード内で#include <JuceHeader.h>とインクルードしている場合に使用します
juce_generate_juce_header(DemoSynthesiser)

# 標準CMakeコマンド: ソースファイルをターゲットに追加します
# GLOB_RECURSE      ...フォルダ内を再帰的にリストアップするGLOB
# CONFIGURE_DEPENDS ...指定することで,ファイルリストが変化するとMakefileを再生成する
file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS ./Source/*.cpp ./Source/*.h)
target_sources(DemoSynthesiser
    PRIVATE
        ${SourceFiles})

# 標準CMakeコマンド: ターゲットのコンパイル時に使用するマクロ定義など
# Projucer同様Preprocessor Definitionsをターゲットに追加
# 特定の機能のON/OFF切り替えが可能
target_compile_definitions(DemoSynthesiser
    PUBLIC
        # JUCE_WEB_BROWSER,JUCE_USE_CURL,JUCE_VST3_CAN_REPLACE_VST2などデフォルトで有効化されているものを無効化する
        DONT_SET_USING_JUCE_NAMESPACE=1         # `using namespace juce`の宣言を無効化する
        JUCE_WEB_BROWSER=0  
        JUCE_USE_CURL=0     
        JUCE_VST3_CAN_REPLACE_VST2=0)

# JUCEモジュールなどを実行可能ファイルにリンク
# JUCEモジュールの場合は依存している他のJUCEモジュールも自動的に含まれる
target_link_libraries(DemoSynthesiser
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)