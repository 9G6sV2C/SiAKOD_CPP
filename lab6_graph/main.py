import tkinter as tk
from tkinter import ttk, filedialog, colorchooser, messagebox
import networkx as nx
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import pandas as pd
import random
from collections import deque
import heapq


class GraphApp:
    """
    GUI-приложение для работы с ориентированными графами.
    """

    def __init__(self, root):
        self.root = root
        self.root.title("GraphMaster Pro")
        self.root.geometry("1400x900")

        # --- Состояние приложения ---
        self.graph = nx.DiGraph()
        self.node_colors = {}
        self.pos = None
        self.nodes_filepath = ""
        self.edges_filepath = ""
        # Для алгоритма Флойда-Уоршелла
        self.dist_matrix = None
        self.next_node_matrix = None

        # --- Основная компоновка ---
        main_pane = ttk.PanedWindow(self.root, orient=tk.HORIZONTAL)
        main_pane.pack(fill=tk.BOTH, expand=True)

        # --- Левая панель управления ---
        controls_frame = ttk.Frame(main_pane, width=400, relief=tk.RIDGE)
        main_pane.add(controls_frame, weight=1)
        self._create_control_widgets(controls_frame)

        # --- Правая панель (граф и вывод) ---
        right_pane = ttk.PanedWindow(main_pane, orient=tk.VERTICAL)
        main_pane.add(right_pane, weight=4)

        # Холст для графа
        self.fig, self.ax = plt.subplots(figsize=(10, 7))
        self.canvas = FigureCanvasTkAgg(self.fig, master=right_pane)
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        right_pane.add(self.canvas.get_tk_widget(), weight=3)

        # Текстовое поле для вывода
        output_frame = ttk.Frame(right_pane)
        self.output_text = tk.Text(output_frame, wrap=tk.WORD, height=15)
        scrollbar = ttk.Scrollbar(output_frame, command=self.output_text.yview)
        self.output_text.config(yscrollcommand=scrollbar.set)
        self.output_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        right_pane.add(output_frame, weight=1)

    def _set_widget_state(self, widget, state):
        """Рекурсивно устанавливает состояние для виджета и всех его дочерних элементов."""
        try:
            widget.config(state=state)
        except tk.TclError:
            pass  # Некоторые виджеты, как Frame, не имеют опции state

        for child in widget.winfo_children():
            self._set_widget_state(child, state)

    def _toggle_creation_mode(self):
        """Переключает активные/неактивные секции создания графа."""
        mode = self.creation_mode.get()
        if mode == "random":
            self._set_widget_state(self.random_frame, 'normal')
            self._set_widget_state(self.file_frame, 'disabled')
        elif mode == "file":
            self._set_widget_state(self.random_frame, 'disabled')
            self._set_widget_state(self.file_frame, 'normal')

    def _create_control_widgets(self, parent):
        """Создает все виджеты на панели управления."""
        container = ttk.Frame(parent)
        container.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # --- Секция создания графа ---
        creation_frame = ttk.LabelFrame(container, text="1. Создание графа")
        creation_frame.pack(fill=tk.X, pady=5)

        self.creation_mode = tk.StringVar(value="random")
        # Добавляем command для переключения состояния
        ttk.Radiobutton(creation_frame, text="Случайный", variable=self.creation_mode, value="random",
                        command=self._toggle_creation_mode).pack(anchor=tk.W)

        # Подсекция случайного графа
        self.random_frame = ttk.Frame(creation_frame)
        self.random_frame.pack(padx=20, fill=tk.X)
        ttk.Label(self.random_frame, text="Вершины:").grid(row=0, column=0, sticky=tk.W)
        self.nodes_entry = ttk.Entry(self.random_frame, width=10)
        self.nodes_entry.insert(0, "10")
        self.nodes_entry.grid(row=0, column=1, padx=5)
        ttk.Label(self.random_frame, text="Рёбра:").grid(row=0, column=2, sticky=tk.W)
        self.edges_entry = ttk.Entry(self.random_frame, width=10)
        self.edges_entry.insert(0, "15")
        self.edges_entry.grid(row=0, column=3, padx=5)
        ttk.Button(self.random_frame, text="Создать", command=self.create_random_graph).grid(row=1, columnspan=4,
                                                                                             pady=5)

        ttk.Radiobutton(creation_frame, text="Из файла", variable=self.creation_mode, value="file",
                        command=self._toggle_creation_mode).pack(anchor=tk.W)

        # Подсекция загрузки из файла
        self.file_frame = ttk.Frame(creation_frame)
        self.file_frame.pack(padx=20, fill=tk.X)
        ttk.Button(self.file_frame, text="Файл вершин (nodes.csv)", command=self.select_nodes_file).pack(fill=tk.X)
        self.nodes_file_label = ttk.Label(self.file_frame, text="Файл не выбран", wraplength=250)
        self.nodes_file_label.pack(fill=tk.X)
        ttk.Button(self.file_frame, text="Файл рёбер (edges.csv)", command=self.select_edges_file).pack(fill=tk.X,
                                                                                                        pady=(5, 0))
        self.edges_file_label = ttk.Label(self.file_frame, text="Файл не выбран", wraplength=250)
        self.edges_file_label.pack(fill=tk.X)
        ttk.Button(self.file_frame, text="Загрузить граф", command=self.load_graph_from_files).pack(pady=5)

        # Устанавливаем начальное состояние
        self._toggle_creation_mode()

        # --- Секция раскраски ---
        color_frame = ttk.LabelFrame(container, text="2. Раскраска вершин")
        color_frame.pack(fill=tk.X, pady=5)
        ttk.Label(color_frame, text="Вершины (через запятую):").pack(anchor=tk.W)
        self.color_nodes_entry = ttk.Entry(color_frame)
        self.color_nodes_entry.pack(fill=tk.X, padx=5, pady=2)
        self.color_btn = ttk.Button(color_frame, text="Выбрать цвет и применить", command=self.apply_color)
        self.color_btn.pack(fill=tk.X, padx=5, pady=5)

        # --- Секция фильтрации ---
        filter_frame = ttk.LabelFrame(container, text="3. Фильтрация вершин")
        filter_frame.pack(fill=tk.X, pady=5)

        ff_grid = ttk.Frame(filter_frame)
        ff_grid.pack(padx=5, pady=5)

        ttk.Label(ff_grid, text="Атрибут:").grid(row=0, column=0, sticky=tk.W)
        self.filter_attr_entry = ttk.Entry(ff_grid, width=12)
        self.filter_attr_entry.insert(0, "age")
        self.filter_attr_entry.grid(row=0, column=1)

        ttk.Label(ff_grid, text="Оператор:").grid(row=0, column=2, sticky=tk.W, padx=(10, 0))
        self.filter_op_combo = ttk.Combobox(ff_grid, values=['==', '!=', '>', '<', '>=', '<='], width=5)
        self.filter_op_combo.set('>')
        self.filter_op_combo.grid(row=0, column=3)

        ttk.Label(ff_grid, text="Значение:").grid(row=1, column=0, sticky=tk.W)
        self.filter_val_entry = ttk.Entry(ff_grid, width=12)
        self.filter_val_entry.insert(0, "30")
        self.filter_val_entry.grid(row=1, column=1)

        ttk.Button(ff_grid, text="Применить фильтр", command=self.apply_filter).grid(row=2, column=0, columnspan=2,
                                                                                     pady=5)
        ttk.Button(ff_grid, text="Сбросить", command=lambda: self.draw_graph()).grid(row=2, column=2, columnspan=2,
                                                                                     pady=5)

        # --- Секция обходов ---
        traversal_frame = ttk.LabelFrame(container, text="4. Обходы графа")
        traversal_frame.pack(fill=tk.X, pady=5)
        ttk.Label(traversal_frame, text="Стартовая вершина:").pack(anchor=tk.W, padx=5)
        self.start_node_entry = ttk.Entry(traversal_frame)
        self.start_node_entry.pack(fill=tk.X, padx=5, pady=2)
        ttk.Button(traversal_frame, text="Обход в ширину (BFS)", command=self.run_bfs).pack(fill=tk.X, padx=5, pady=2)
        ttk.Button(traversal_frame, text="Обход в глубину (DFS)", command=self.run_dfs).pack(fill=tk.X, padx=5, pady=2)

        # --- Секция поиска путей ---
        path_frame = ttk.LabelFrame(container, text="5. Поиск кратчайших путей")
        path_frame.pack(fill=tk.X, pady=5)
        pf_grid = ttk.Frame(path_frame)
        pf_grid.pack(padx=5, pady=5)
        ttk.Label(pf_grid, text="Начало:").grid(row=0, column=0, sticky=tk.W)
        self.source_node_entry = ttk.Entry(pf_grid, width=10)
        self.source_node_entry.grid(row=0, column=1)
        ttk.Label(pf_grid, text="Конец:").grid(row=0, column=2, sticky=tk.W, padx=(10, 0))
        self.target_node_entry = ttk.Entry(pf_grid, width=10)
        self.target_node_entry.grid(row=0, column=3)
        ttk.Button(path_frame, text="Алгоритм Дейкстры", command=self.run_dijkstra).pack(fill=tk.X, padx=5, pady=2)
        ttk.Button(path_frame, text="Алгоритм Флойда-Уоршелла", command=self.run_floyd_warshall).pack(fill=tk.X, padx=5,
                                                                                                      pady=2)

    def _log(self, message):
        """Добавляет сообщение в текстовое поле вывода."""
        self.output_text.insert(tk.END, message + "\n")
        self.output_text.see(tk.END)

    def _validate_node(self, node_name, graph=None):
        """Проверяет, существует ли вершина в графе."""
        g = graph if graph is not None else self.graph
        try:
            # Пытаемся преобразовать в int, если вершины числовые
            node = int(node_name)
        except (ValueError, TypeError):
            node = node_name

        if node not in g.nodes:
            messagebox.showerror("Ошибка", f"Вершина '{node_name}' не найдена в графе.")
            return None
        return node

    def draw_graph(self, graph_to_draw=None):
        """Отрисовывает граф на холсте."""
        g = graph_to_draw if graph_to_draw is not None else self.graph
        self.ax.clear()

        if not g.nodes:
            self.ax.text(0.5, 0.5, "Граф пуст. Создайте или загрузите граф.", ha='center', va='center')
            self.canvas.draw()
            return

        self.pos = nx.circular_layout(g)

        colors = [self.node_colors.get(node, '#1f78b4') for node in g.nodes()]
        edge_labels = nx.get_edge_attributes(g, 'weight')

        # --- ИЗМЕНЕНИЕ 2: Формирование детализированных подписей ---
        custom_labels = {}
        for node, data in g.nodes(data=True):
            # Начинаем с id
            label_parts = [f"id: {node}"]
            # Добавляем остальные атрибуты
            for key, value in data.items():
                if key != 'id':  # 'id' уже есть в самом идентификаторе узла
                    label_parts.append(f"{key}: {value}")
            custom_labels[node] = "\n".join(label_parts)

        nx.draw_networkx_nodes(g, self.pos, ax=self.ax, node_color=colors, node_size=2000)

        # --- ИЗМЕНЕНИЕ 3: Отображение стрелок (было и раньше, но подтверждено) ---
        nx.draw_networkx_edges(g, self.pos, ax=self.ax, arrowstyle='->', arrowsize=20, connectionstyle='arc3,rad=0.1',
                               node_size=2000)

        # Используем новые подписи и настраиваем их внешний вид
        nx.draw_networkx_labels(g, self.pos, labels=custom_labels, ax=self.ax, font_size=8, font_color='black',
                                bbox=dict(facecolor='white', alpha=0.7, edgecolor='none', boxstyle='round,pad=0.2'))

        nx.draw_networkx_edge_labels(g, self.pos, edge_labels=edge_labels, ax=self.ax, font_size=9)

        self.ax.set_title("Визуализация графа")
        self.ax.margins(0.1)  # Добавляем отступы
        self.ax.set_axis_off()
        self.canvas.draw()

    # --- Функции создания графа ---
    def create_random_graph(self):
        """
        Создает случайный граф и добавляет несколько атрибутов
        к его вершинам и ребрам для демонстрации.
        """
        try:
            n = int(self.nodes_entry.get())
            m = int(self.edges_entry.get())
            if n <= 0 or m < 0:
                raise ValueError
        except ValueError:
            messagebox.showerror("Ошибка", "Количество вершин и рёбер должно быть положительными числами.")
            return

        self.graph = nx.gnp_random_graph(n, min(1.0, m / (n * (n - 1) if n > 1 else 1)), directed=True)
        # Добавляем атрибуты для вершин
        for i in self.graph.nodes():
            self.graph.nodes[i]['group'] = random.choice(['A', 'B'])
            self.graph.nodes[i]['value'] = random.randint(1, 100)

        # ИЗМЕНЕНИЕ: Добавляем несколько атрибутов к ребрам
        for (u, v) in self.graph.edges():
            self.graph.edges[u, v]['weight'] = random.randint(1, 20)
            self.graph.edges[u, v]['relationship'] = random.choice(['work', 'social', 'family'])

        self.node_colors.clear()
        self._log(f"Создан случайный граф с {n} вершинами и {self.graph.number_of_edges()} рёбрами.")
        self.draw_graph()

    def draw_graph(self, graph_to_draw=None):
        """Отрисовывает граф на холсте с детализированными подписями для вершин и ребер."""
        g = graph_to_draw if graph_to_draw is not None else self.graph
        self.ax.clear()

        if not g.nodes:
            self.ax.text(0.5, 0.5, "Граф пуст. Создайте или загрузите граф.", ha='center', va='center')
            self.canvas.draw()
            return

        self.pos = nx.circular_layout(g)

        colors = [self.node_colors.get(node, '#1f78b4') for node in g.nodes()]

        # --- НАЧАЛО ИЗМЕНЕНИЙ ---
        # Формируем подписи для ребер, включая все их атрибуты
        edge_labels = {}
        for u, v, data in g.edges(data=True):
            # Собираем все атрибуты ребра в одну строку с переносами
            label = "\n".join(f"{key}: {value}" for key, value in data.items())
            edge_labels[(u, v)] = label
        # --- КОНЕЦ ИЗМЕНЕНИЙ ---

        # Формирование детализированных подписей для вершин
        custom_node_labels = {}
        for node, data in g.nodes(data=True):
            label_parts = [f"id: {node}"]
            for key, value in data.items():
                if key != 'id':
                    label_parts.append(f"{key}: {value}")
            custom_node_labels[node] = "\n".join(label_parts)

        # Отрисовка элементов графа
        nx.draw_networkx_nodes(g, self.pos, ax=self.ax, node_color=colors, node_size=2500)
        nx.draw_networkx_edges(g, self.pos, ax=self.ax, arrowstyle='->', arrowsize=20, connectionstyle='arc3,rad=0.1',
                               node_size=2500)

        # Отрисовка подписей вершин с фоном для читаемости
        nx.draw_networkx_labels(g, self.pos, labels=custom_node_labels, ax=self.ax, font_size=8, font_color='black',
                                bbox=dict(facecolor='white', alpha=0.7, edgecolor='none', boxstyle='round,pad=0.3'))

        # --- НАЧАЛО ИЗМЕНЕНИЙ ---
        # Отрисовка новых, многострочных подписей ребер
        nx.draw_networkx_edge_labels(
            g,
            self.pos,
            edge_labels=edge_labels,
            ax=self.ax,
            font_size=7,
            font_color='darkred',
            bbox=dict(facecolor='white', alpha=0.5, edgecolor='none', boxstyle='round,pad=0.2')
        )
        # --- КОНЕЦ ИЗМЕНЕНИЙ ---

        self.ax.set_title("Визуализация графа")
        self.ax.margins(0.2)  # Увеличиваем отступы для лучшей видимости подписей
        self.ax.set_axis_off()
        self.canvas.draw()

    def select_nodes_file(self):
        self.nodes_filepath = filedialog.askopenfilename(filetypes=[("CSV files", "*.csv")])
        if self.nodes_filepath:
            self.nodes_file_label.config(text=self.nodes_filepath.split('/')[-1])

    def select_edges_file(self):
        self.edges_filepath = filedialog.askopenfilename(filetypes=[("CSV files", "*.csv")])
        if self.edges_filepath:
            self.edges_file_label.config(text=self.edges_filepath.split('/')[-1])

    def load_graph_from_files(self):
        if not self.nodes_filepath or not self.edges_filepath:
            messagebox.showerror("Ошибка", "Пожалуйста, выберите оба файла (вершины и рёбра).")
            return

        try:
            nodes_df = pd.read_csv(self.nodes_filepath)
            edges_df = pd.read_csv(self.edges_filepath)

            g = nx.DiGraph()

            # Добавление вершин с атрибутами
            for _, row in nodes_df.iterrows():
                node_id = row['id']
                attrs = row.to_dict()
                g.add_node(node_id, **attrs)

            # Добавление рёбер с атрибутами
            for _, row in edges_df.iterrows():
                source = row['source']
                target = row['target']
                attrs = row.to_dict()
                # Удаляем ключи, которые не являются атрибутами ребра
                attrs.pop('source')
                attrs.pop('target')
                if 'weight' not in attrs:
                    attrs['weight'] = 1  # Вес по умолчанию
                g.add_edge(source, target, **attrs)

            self.graph = g
            self.node_colors.clear()
            self._log(f"Граф успешно загружен из файлов.\nВершин: {g.number_of_nodes()}, Рёбер: {g.number_of_edges()}")
            self.draw_graph()
        except Exception as e:
            messagebox.showerror("Ошибка загрузки", f"Не удалось загрузить граф из файлов:\n{e}")

    # --- Функции визуализации и фильтрации ---
    def apply_color(self):
        color_info = colorchooser.askcolor(title="Выберите цвет")
        if not color_info or not color_info[1]:
            return

        color_hex = color_info[1]
        nodes_str = self.color_nodes_entry.get()
        if not nodes_str:
            messagebox.showwarning("Внимание", "Введите вершины для раскраски.")
            return

        node_list_str = nodes_str.split(',')
        for node_str in node_list_str:
            node = self._validate_node(node_str.strip())
            if node is not None:
                self.node_colors[node] = color_hex

        self.draw_graph()
        self._log(f"Применён цвет {color_hex} к вершинам: {nodes_str}")

    def apply_filter(self):
        attr = self.filter_attr_entry.get()
        op_str = self.filter_op_combo.get()
        val_str = self.filter_val_entry.get()

        if not all([attr, op_str, val_str]):
            messagebox.showerror("Ошибка", "Все поля фильтра должны быть заполнены.")
            return

        filtered_nodes = []
        for node, data in self.graph.nodes(data=True):
            if attr in data:
                node_val = data[attr]
                try:
                    # Попытка преобразовать значение фильтра к типу атрибута
                    filter_val = type(node_val)(val_str)

                    if op_str == '==' and node_val == filter_val:
                        filtered_nodes.append(node)
                    elif op_str == '!=' and node_val != filter_val:
                        filtered_nodes.append(node)
                    elif op_str == '>' and node_val > filter_val:
                        filtered_nodes.append(node)
                    elif op_str == '<' and node_val < filter_val:
                        filtered_nodes.append(node)
                    elif op_str == '>=' and node_val >= filter_val:
                        filtered_nodes.append(node)
                    elif op_str == '<=' and node_val <= filter_val:
                        filtered_nodes.append(node)

                except (ValueError, TypeError):
                    # Если типы несовместимы (например, сравнение строки с числом), пропускаем
                    if op_str == '==' and str(node_val) == val_str:
                        filtered_nodes.append(node)
                    elif op_str == '!=' and str(node_val) != val_str:
                        filtered_nodes.append(node)

        if not filtered_nodes:
            messagebox.showinfo("Фильтр", "Ни одна вершина не соответствует условию.")
            return

        subgraph = self.graph.subgraph(filtered_nodes)
        self.draw_graph(graph_to_draw=subgraph)
        self._log(f"Применён фильтр: '{attr} {op_str} {val_str}'. Найдено {len(filtered_nodes)} вершин.")

    # --- Алгоритмы ---
    def _my_bfs(self, start_node):
        if start_node not in self.graph: return []
        q = deque([start_node])
        visited = {start_node}
        traversal_order = []
        while q:
            node = q.popleft()
            traversal_order.append(node)
            for neighbor in self.graph.neighbors(node):
                if neighbor not in visited:
                    visited.add(neighbor)
                    q.append(neighbor)
        return traversal_order

    def _my_dfs(self, start_node):
        if start_node not in self.graph: return []
        stack = [start_node]
        visited = {start_node}
        traversal_order = []
        while stack:
            node = stack.pop()
            traversal_order.append(node)
            for neighbor in reversed(list(self.graph.neighbors(node))):
                if neighbor not in visited:
                    visited.add(neighbor)
                    stack.append(neighbor)
        return traversal_order

    def run_bfs(self):
        start_node = self._validate_node(self.start_node_entry.get())
        if start_node is None: return

        self.output_text.delete(1.0, tk.END)
        self._log(f"--- Обход в ширину (BFS) от вершины {start_node} ---")

        my_result = self._my_bfs(start_node)
        self._log(f"Своя реализация: {my_result}")

        try:
            nx_result = list(nx.bfs_tree(self.graph, source=start_node).nodes())
            self._log(f"Реализация NetworkX: {nx_result}")
            self._log(f"Сравнение: {'Результаты совпадают' if my_result == nx_result else 'Результаты различаются'}")
        except nx.NetworkXError as e:
            self._log(f"NetworkX не смог выполнить обход: {e}")

    def run_dfs(self):
        start_node = self._validate_node(self.start_node_entry.get())
        if start_node is None: return

        self.output_text.delete(1.0, tk.END)
        self._log(f"--- Обход в глубину (DFS) от вершины {start_node} ---")

        my_result = self._my_dfs(start_node)
        self._log(f"Своя реализация: {my_result}")

        try:
            nx_result = list(nx.dfs_tree(self.graph, source=start_node).nodes())
            self._log(f"Реализация NetworkX: {nx_result}")
            self._log(
                f"Сравнение (по составу): {'Состав вершин совпадает' if set(my_result) == set(nx_result) else 'Состав вершин различается'}")
        except nx.NetworkXError as e:
            self._log(f"NetworkX не смог выполнить обход: {e}")

    def _my_dijkstra(self, start, end):
        if start not in self.graph or end not in self.graph:
            return None, float('inf')

        distances = {node: float('inf') for node in self.graph.nodes}
        distances[start] = 0
        predecessors = {node: None for node in self.graph.nodes}

        pq = [(0, start)]  # (distance, node)

        while pq:
            current_dist, current_node = heapq.heappop(pq)

            if current_dist > distances[current_node]:
                continue

            if current_node == end:
                break

            for neighbor in self.graph.neighbors(current_node):
                weight = self.graph.edges[current_node, neighbor].get('weight', 1)
                distance = current_dist + weight

                if distance < distances[neighbor]:
                    distances[neighbor] = distance
                    predecessors[neighbor] = current_node
                    heapq.heappush(pq, (distance, neighbor))

        path = []
        node = end
        if distances[node] == float('inf'):
            return None, float('inf')

        while node is not None:
            path.insert(0, node)
            node = predecessors[node]

        if path[0] == start:
            return path, distances[end]
        else:
            return None, float('inf')

    def run_dijkstra(self):
        start_node = self._validate_node(self.source_node_entry.get())
        end_node = self._validate_node(self.target_node_entry.get())
        if start_node is None or end_node is None: return

        self.output_text.delete(1.0, tk.END)
        self._log(f"--- Поиск пути (Дейкстра) из {start_node} в {end_node} ---")

        my_path, my_cost = self._my_dijkstra(start_node, end_node)
        if my_path:
            self._log(f"Своя реализация: Путь = {my_path}, Стоимость = {my_cost}")
        else:
            self._log("Своя реализация: Путь не найден.")

        try:
            nx_path = nx.dijkstra_path(self.graph, source=start_node, target=end_node, weight='weight')
            nx_cost = nx.dijkstra_path_length(self.graph, source=start_node, target=end_node, weight='weight')
            self._log(f"Реализация NetworkX: Путь = {nx_path}, Стоимость = {nx_cost}")
            self._log(
                f"Сравнение: {'Результаты совпадают' if my_path == nx_path and my_cost == nx_cost else 'Результаты различаются'}")
        except nx.NetworkXNoPath:
            self._log("Реализация NetworkX: Путь не найден.")

    def _my_floyd_warshall(self):
        nodes = list(self.graph.nodes)
        node_map = {node: i for i, node in enumerate(nodes)}
        n = len(nodes)

        self.dist_matrix = [[float('inf')] * n for _ in range(n)]
        self.next_node_matrix = [[None] * n for _ in range(n)]

        for i in range(n):
            self.dist_matrix[i][i] = 0

        for u, v, data in self.graph.edges(data=True):
            i, j = node_map[u], node_map[v]
            weight = data.get('weight', 1)
            self.dist_matrix[i][j] = weight
            self.next_node_matrix[i][j] = j

        for k_idx, k in enumerate(nodes):
            for i_idx, i in enumerate(nodes):
                for j_idx, j in enumerate(nodes):
                    if self.dist_matrix[i_idx][j_idx] > self.dist_matrix[i_idx][k_idx] + self.dist_matrix[k_idx][j_idx]:
                        self.dist_matrix[i_idx][j_idx] = self.dist_matrix[i_idx][k_idx] + self.dist_matrix[k_idx][j_idx]
                        self.next_node_matrix[i_idx][j_idx] = self.next_node_matrix[i_idx][k_idx]

    def _get_my_fw_path(self, u, v):
        nodes = list(self.graph.nodes)
        node_map = {node: i for i, node in enumerate(nodes)}

        u_idx, v_idx = node_map.get(u), node_map.get(v)
        if u_idx is None or v_idx is None or self.next_node_matrix is None or self.next_node_matrix[u_idx][
            v_idx] is None:
            return None, float('inf')

        path = [u]
        curr = u
        while curr != v:
            curr_idx = node_map[curr]
            next_node_idx = self.next_node_matrix[curr_idx][v_idx]
            if next_node_idx is None: return None, float('inf')  # Разрыв
            curr = nodes[next_node_idx]
            path.append(curr)

        return path, self.dist_matrix[u_idx][v_idx]

    def run_floyd_warshall(self):
        start_node = self._validate_node(self.source_node_entry.get())
        end_node = self._validate_node(self.target_node_entry.get())
        if start_node is None or end_node is None: return

        self.output_text.delete(1.0, tk.END)
        self._log(f"--- Поиск пути (Флойд-Уоршелл) из {start_node} в {end_node} ---")

        # Запуск своей реализации
        self._log("Выполнение собственной реализации (вычисляет все пути)...")
        self._my_floyd_warshall()
        my_path, my_cost = self._get_my_fw_path(start_node, end_node)
        if my_path:
            self._log(f"Своя реализация: Путь = {my_path}, Стоимость = {my_cost}")
        else:
            self._log("Своя реализация: Путь не найден.")

        # Запуск реализации NetworkX
        try:
            self._log("Выполнение реализации NetworkX (вычисляет все пути)...")
            nx_preds, nx_dists = nx.floyd_warshall_predecessor_and_distance(self.graph, weight='weight')
            nx_path = nx.reconstruct_path(start_node, end_node, nx_preds)
            nx_cost = nx_dists[start_node][end_node]
            self._log(f"Реализация NetworkX: Путь = {nx_path}, Стоимость = {nx_cost}")
            self._log(
                f"Сравнение: {'Результаты совпадают' if my_path == nx_path and my_cost == nx_cost else 'Результаты различаются'}")
        except nx.NetworkXNoPath:
            self._log("Реализация NetworkX: Путь не найден.")
        except Exception as e:
            self._log(f"Ошибка в NetworkX: {e}")


if __name__ == "__main__":
    # --- Примерные файлы для тестирования ---
    # Создаем тестовые CSV файлы, если они не существуют
    try:
        with open("nodes.csv", "w+") as f:
            f.write("id,name,city,age\n")
            f.write("1,Ivan,Moscow,30\n")
            f.write("2,Maria,StP,25\n")
            f.write("3,Peter,Moscow,45\n")
            f.write("4,Anna,Novosibirsk,28\n")
            f.write("5,Sergey,StP,35\n")
    except FileExistsError:
        pass  # Файл уже существует

    try:
        with open("edges.csv", "w+") as f:
            f.write("source,target,weight,relationship\n")
            f.write("1,2,5,colleague\n")
            f.write("1,3,10,family\n")
            f.write("2,3,3,acquaintance\n")
            f.write("2,5,8,colleague\n")
            f.write("4,1,7,friend\n")
            f.write("4,5,12,friend\n")
            f.write("3,4,4,acquaintance\n")
    except FileExistsError:
        pass  # Файл уже существует

    root = tk.Tk()
    app = GraphApp(root)
    root.mainloop()