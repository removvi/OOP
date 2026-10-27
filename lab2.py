import tkinter as tk
import module1
import module2


def main():
    root = tk.Tk()
    root.title("Лабораторна №1 — варіант 16")
    root.geometry("520x280")

    result = tk.StringVar(value="Вибери «Робота1» або «Робота2» в меню.")

    def work1():
        text = module1.show_dialog(root)
        if text is not None:
            result.set(f"Введений текст:\n{text}")

    def work2():
        number = module2.show_dialog(root)
        if number is not None:
            result.set(f"Вибране число: {number}")

    menu = tk.Menu(root)
    menu.add_command(label="Робота1", command=work1)
    menu.add_command(label="Робота2", command=work2)
    root.config(menu=menu)

    tk.Label(
        root,
        textvariable=result,
        font=("Arial", 14),
        wraplength=460,
    ).pack(expand=True, padx=20, pady=20)

    root.mainloop()


if __name__ == "__main__":
    main()
