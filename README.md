# kuview 3D ビューアー

フラグメントシェーダーをユーザーが自由に編集できる 3D ビューアーです。

## 依存関係

ビルドには Qt5, Assimp が必要です。
Visual Studio 2017 Community でビルドを確認しています。

## 機能

* フラグメントシェーダーをユーザーが改変できます。
* Epic の Metallic, Roughness テクスチャーを利用した PBR シェーダーを標準で搭載しています。
* ビューポートをマウス、キーボードで操作できます。

### カメラ操作

* W,A,S,D: それぞれ、前進、左に移動、右に移動、後退
* QE: それぞれ、上昇、下降
* F: モデルの中央にフォーカス
* マウス - ドラッグ: モデルを中心に旋回 

## 備考

* glsls.h ファイルにデフォルトの glsl を記載しています。
* 名前規則は C++ Core Guidelines, Google C++ Styleguide を参考にしています。

[Cpp Core Guidelines]("https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md")

[Google C++ Style Guide]("https://google.github.io/styleguide/cppguide.html")