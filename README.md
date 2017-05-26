# vimdows

Windows-native Vim plugin for better GUI experience

## How to get or build

This is optional. A pre-built 64-bit binary is available on [releases](https://github.com/GrzegorzKozub/vimdows/releases) page. If you want to build `vimdows.dll` yourself with [mingw-w64](http://mingw-w64.org/) use the following command:

`g++ vimdows.cpp -o vimdows.dll -shared -lgdi32 -O3 -s -static`

## How to install

Copy `vimdows.dll` to your Vim installation directory. The same place where `gvim.exe` is. Then, the following snippet gives you a convenient abstraction over functions exported by `vimdows.dll`. Place it in your `vimrc`.

```
if has('win32') && has('gui_running')

    let s:vimdows_file = $VIMRUNTIME . '/vimdows.dll'

    function! GetScreen()
        return libcall(s:vimdows_file, 'GetScreen', '')
    endfunction

    function! GetMaximized()
        return libcallnr(s:vimdows_file, 'GetMaximized', '')
    endfunction

    function! GetFullScreen()
        return libcallnr(s:vimdows_file, 'GetFullScreen', '')
    endfunction

    function! Maximize()
        call libcall(s:vimdows_file, 'Maximize', '')
    endfunction

    function! Restore()
        call libcall(s:vimdows_file, 'Restore', '')
    endfunction

    function! EnterFullScreen()
        call libcall(s:vimdows_file, 'EnterFullScreen', '')
    endfunction

    function! ExitFullScreen()
        call libcall(s:vimdows_file, 'ExitFullScreen', '')
    endfunction

    function! FixBackground()
        call libcall(s:vimdows_file, 'FixBackground', strpart(synIDattr(hlID('Normal'), 'bg#'), 1))
    endfunction

endif
```

## How to use

The plugin provides functions that aim to make GVim experience under Windows nicer. Let's take a closer look at each of them now.

### GetScreen

Returns a JSON object that describes the current screen resolution and scaling: `{ 'width': 3200, 'height': 1800, 'dpi': 240 }`. Example use:

```
if has('win32') && has('gui_running')
    let s:screen = eval(GetScreen())
    if s:screen.height == 1800 && s:screen.dpi == 240
        set guifont=Fira\ Code\ Retina:h13
        set columns=117
        set lines=29
        winpos 63 66
    endif
endif
```

### GetMaximized and GetFullScreen

Return `1` if GVim is, respectively, maximized or in full screen and `0` otherwise. Examples will follow.

### Maximize and Restore

The former maximizes GVim window and the latter restores it. I like to use `F10` to switch between maximized and restored:

```
if has('win32') && has('gui_running')
    function! MaximizedToggle()
        if GetFullScreen() | return | endif
        if GetMaximized() | call Restore() | else | call Maximize() | endif
    endfunction
    nmap <silent> <F10> :call MaximizedToggle()<CR>
endif
```

Light frame on the right and the bottom fix will follow. 

### EnterFullScreen and ExitFullScreen

Quite obvious what they do. The code under the hood takes inspiration from [Raymond Chen](https://blogs.msdn.microsoft.com/oldnewthing/20100412-00/?p=14353). I like to match Chrome and Visual Studio Code shortcut here, so I use `F11` for a handy toggle:

```
if has('win32') && has('gui_running')
    function! FullScreenToggle()
        if GetFullScreen() | call ExitFullScreen() | else | call EnterFullScreen() | endif
    endfunction
    nmap <silent> <F11> :call FullScreenToggle()<CR>
endif
```

Light frame on the right and the bottom fix will follow. 

### FixBackground

When GVim is maximized or in full screen there's a frame on the right side and on the bottom side of the text area that does not match current GVim theme. To correct this we need to call `FixBackground` as wrapped above (passing a hex representation of the current background color). The fix needs to run every time GVim gets maximized or enters full screen. That's the first event handler. The other one caters for changing themes when maximized or in full screen.

```
if has('win32') && has('gui_running')
    augroup FixBackgroundWhenColorSchemeChanges
        autocmd!
        autocmd ColorScheme * call FixBackground()
    augroup END
    augroup FixBackgroundWhenResizing
        autocmd!
        autocmd VimResized * if GetMaximized() || GetFullScreen() | call FixBackground() | endif
    augroup END
endif
```

