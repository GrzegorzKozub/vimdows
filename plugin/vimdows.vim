if &compatible || !has('win32') || !has('gui_running') || exists('g:vimdows_plugin_loaded')
    finish
endif

if !exists('g:vimdows_mappings')
    let g:vimdows_mappings = 1
endif

if g:vimdows_mappings
    nmap <silent> <F10> :call vimdows#maximized_toggle()<CR>
    nmap <silent> <F11> :call vimdows#full_screen_toggle()<CR>
endif

if !exists('g:vimdows_background_fixes')
    let g:vimdows_background_fixes = 1
endif

if g:vimdows_background_fixes
    augroup FixBackgroundWhenColorSchemeChanges
        autocmd!
        autocmd ColorScheme * call vimdows#fix_background()
    augroup END
    augroup FixBackgroundWhenResizing
        autocmd!
        autocmd VimResized * if vimdows#get_maximized() || vimdows#get_full_screen() | call vimdows#fix_background() | endif
    augroup END
endif

if !exists('g:vimdows_screen_memento')
    let g:vimdows_screen_memento = 1
endif

if g:vimdows_screen_memento
    augroup InitScreenMementoWhenVimEnters
        autocmd!
        autocmd VimEnter * call vimdows#init_screen_memento()
    augroup END
    augroup SaveScreenWhenVimLeaves
        autocmd!
        autocmd VimLeavePre * call vimdows#save_screen()
    augroup END
    augroup RestoreScreenWhenGUIEnters
        autocmd!
        autocmd GUIEnter * call vimdows#restore_screen(1)
    augroup END
endif

let g:vimdows_plugin_loaded = 1

