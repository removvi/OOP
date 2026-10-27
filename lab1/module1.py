import tkinter as tk


def show_dialog(parent):
    result = None

    dialog = tk.Toplevel(parent)
    dialog.title("Робота1 — введення тексту")
    dialog.geometry("400x170")
    dialog.resizable(False, False)
    dialog.transient(parent)

    tk.Label(dialog, text="Введи текст:").pack(pady=(15, 5))

    entry = tk.Entry(dialog, width=45)
    entry.pack(padx=15, pady=5)

    def accept():
        nonlocal result
        result = entry.get()
        dialog.destroy()

    def cancel():
        dialog.destroy()

    buttons = tk.Frame(dialog)
    buttons.pack(pady=15)

    tk.Button(
        buttons, text="Так", width=12, command=accept
    ).pack(side=tk.LEFT, padx=5)

    tk.Button(
        buttons, text="Відміна", width=12, command=cancel
    ).pack(side=tk.LEFT, padx=5)

    dialog.protocol("WM_DELETE_WINDOW", cancel)
    dialog.bind("<Return>", lambda event: accept())
    dialog.bind("<Escape>", lambda event: cancel())

    dialog.wait_visibility()
    dialog.grab_set()
    entry.focus_set()
    parent.wait_window(dialog)

    return result
