# What is it

Vim plugin that makes GUI experience better under Windows:

- automates maximizing and restoring the window
- provides full screen functionality
- fixes the lower-right window border when maximized
- remembers the window position and size

# How to install

## Plugin installation

Use your favorite plugin manager. I prefer [vim-plug](https://github.com/junegunn/vim-plug):

```
Plug 'GrzegorzKozub/vimdows'
```

## Required vimrc changes

This plugin uses `g:VIMDOWS_SCREEN` variable stored in `viminfo` to save and restore the window position and size. This depends on the following lines added somewhere to the top of your `vimrc`. The first one enables GVim to store all-caps global variables in `viminfo` and the other one reads it early:

```
let &viminfo = &viminfo . ',!'
try | rviminfo | catch | endtry
```

If you don't like this, set `g:vimdows_screen_memento` to `0` to disable this functionality.

# Usage

The plugin provides functions that aim to make GVim experience under Windows nicer. By default, some of them are mapped to keys and some run in a response to GVim events.

## vimdows#get_screen

Returns a JSON object that describes the current screen resolution and scaling: `{ 'width': 3200, 'height': 1800, 'dpi': 240 }`. Example use:

```
if has('win32') && has('gui_running')
    let s:screen = eval(vimdows#get_screen())
    if s:screen.height == 1800 && s:screen.dpi == 240
        set guifont=Fira\ Code\ Retina:h13
        set columns=117
        set lines=29
        winpos 63 66
    endif
endif
```

## vimdows#get_maximized and vimdows#get_full_screen

Return `1` if GVim is, respectively, maximized or in full screen and `0` otherwise.

## vimdows#maximize and vimdows#restore

The former maximizes GVim window and the latter restores it.

## vimdows#enter_full_screen and vimdows#exit_full_screen

Full screen mode hides title bar, menu, toolbar and taskbar.

## vimdows#maximized_toggle and vimdows#full_screen_toggle

These are the toggle variants of the above functions. `F10` will toggle maximized and `F11` will toggle full screen. If you don't like these mappings, disable them in your `vimrc`:

```
let g:vimdows_mappings = 0
```

## vimdows#fix_background

When GVim is maximized or in full screen there's a bright frame on the right side and on the bottom side of the text area that does not match current GVim theme. To correct this you can use `vimdows#fix_background`.

This function is called every time GVim gets maximized or enters full screen and also when changing themes when maximized or in full screen. You can opt-out in your `vimrc` by:

```
let g:vimdows_background_fixes = 0
```

Then you will be able to use this function yourself.

## vimdows#save_screen and vimdows#restore_screen(allowFullScreen)

In order to work, this requires the `vimrc` changes described above. The following GVim window parameters are saved: position, size, is it maximized, is it full screen. The `allowFullScreen` parameter can be used to force GVim to go out of full screen on next launch.

Your window will be saved when you exit GVim and restored when you launch GVim next time. You can disable this in your `vimrc` like so:

```
let g:vimdows_screen_memento = 0
```

# References

Lots of inspiration and some code taken from:

- [vim-shell](https://github.com/xolox/vim-shell/)
- [gvimfullscreen](https://github.com/xqin/gvimfullscreen/)
- [How do I switch a window between normal and fullscreen?](https://blogs.msdn.microsoft.com/oldnewthing/20100412-00/?p=14353)
