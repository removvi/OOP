import tkinter as tk


def show_dialog(parent):
    result = None

    dialog = tk.Toplevel(parent)
    dialog.title("Робота2 — вибір числа")
    dialog.geometry("400x190")
    dialog.resizable(False, False)
    dialog.transient(parent)

    value = tk.IntVar(value=1)

    tk.Label(
        dialog, text="Вибери число від 1 до 100:"
    ).pack(pady=(15, 5))

    tk.Label(
        dialog, textvariable=value, font=("Arial", 16)
    ).pack(pady=5)

    def scroll(action, *args):
        if action == "moveto":
            number = round(float(args[0]) * 100) + 1
        elif action == "scroll":
            step = 10 if args[1] == "pages" else 1
            number = value.get() + int(args[0]) * step
        else:
            return

        number = max(1, min(100, number))
        value.set(number)
        scrollbar.set((number - 1) / 100, number / 100)

    scrollbar = tk.Scrollbar(
        dialog,
        orient=tk.HORIZONTAL,
        command=scroll,
    )
    scrollbar.pack(fill=tk.X, padx=25, pady=10)
    scrollbar.set(0.0, 0.01)

    def accept():
        nonlocal result
        result = value.get()
        dialog.destroy()

    def cancel():
        dialog.destroy()

    buttons = tk.Frame(dialog)
    buttons.pack(pady=10)

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
    parent.wait_window(dialog)

    return result
