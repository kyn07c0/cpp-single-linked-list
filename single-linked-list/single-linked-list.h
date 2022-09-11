#pragma once

#include <cassert>
#include <cstddef>
#include <string>
#include <utility>

template <typename Type>
class SingleLinkedList 
{
    struct Node
    {
        Node() = default;

        Node(const Type& val, Node* next) : value(val), next_node(next) {}

        Type value;
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator
    {
        public:
            // Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора

            // Категория итератора — forward iterator
            // (итератор, который поддерживает операции инкремента и многократное разыменование)
            using iterator_category = std::forward_iterator_tag;
            // Тип элементов, по которым перемещается итератор
            using value_type = Type;
            // Тип, используемый для хранения смещения между итераторами
            using difference_type = std::ptrdiff_t;
            // Тип указателя на итерируемое значение
            using pointer = ValueType*;
            // Тип ссылки на итерируемое значение
            using reference = ValueType&;

            BasicIterator() = default;

            // Конвертирующий конструктор/конструктор копирования
            // При ValueType, совпадающем с Type, играет роль копирующего конструктора
            // При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
            BasicIterator(const BasicIterator<Type>& other) noexcept
            {
                node_ = other.node_;
            }

            // Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
            // пользовательского конструктора копирования, явно объявим оператор = и
            // попросим компилятор сгенерировать его за нас
            BasicIterator& operator=(const BasicIterator& rhs) = default;

            // Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
            // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
            [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept
            {
                return node_ == rhs.node_;
            }

            // Оператор проверки итераторов на неравенство
            // Противоположен !=
            [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept
            {
                return !(*this == rhs);
            }

            // Оператор сравнения итераторов (в роли второго аргумента итератор)
            // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
            [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept
            {
                return node_ == rhs.node_;
            }

            // Оператор проверки итераторов на неравенство
            // Противоположен !=
            [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept
            {
                return !(*this == rhs);
            }

            // Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
            // Возвращает ссылку на самого себя
            // Инкремент итератора, не указывающего на существующий элемент списка, приводит к неопределённому поведению
            BasicIterator& operator++() noexcept
            {
                if(node_ != nullptr)
                {
                    node_ = node_->next_node;
                }

                return *this;
            }

            // Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка
            // Возвращает прежнее значение итератора
            // Инкремент итератора, не указывающего на существующий элемент списка,
            // приводит к неопределённому поведению
            BasicIterator operator++(int) noexcept
            {
                auto old_value(*this);  // Сохраняем прежнее значение объекта для последующего возврата
                ++(*this);              // используем логику префиксной формы инкремента
                return old_value;
            }

            // Операция разыменования. Возвращает ссылку на текущий элемент
            // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
            // приводит к неопределённому поведению
            [[nodiscard]] reference operator*() const noexcept
            {
                return node_->value;
            }

            // Операция доступа к члену класса. Возвращает указатель на текущий элемент списка
            // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
            // приводит к неопределённому поведению
            [[nodiscard]] pointer operator->() const noexcept
            {
                return &(node_->value);
            }

        private:
            Node* node_ = nullptr;

            // Класс списка объявляется дружественным, чтобы из методов списка
            // был доступ к приватной области итератора
            friend class SingleLinkedList;

            // Конвертирующий конструктор итератора из указателя на узел списка
            explicit BasicIterator(Node* node) : node_(node) {}
    };

    public:
        using value_type = Type;
        using reference = value_type&;
        using const_reference = const value_type&;

        // Итератор, допускающий изменение элементов списка
        using Iterator = BasicIterator<Type>;
        // Константный итератор, предоставляющий доступ для чтения к элементам списка
        using ConstIterator = BasicIterator<const Type>;

        // Возвращает итератор, ссылающийся на первый элемент
        // Если список пустой, возвращённый итератор будет равен end()
        [[nodiscard]] Iterator begin() noexcept
        {
            return Iterator{head_.next_node};
        }

        // Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
        // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
        [[nodiscard]] Iterator end() noexcept
        {
            return Iterator{nullptr};
        }

        // Возвращает константный итератор, ссылающийся на первый элемент
        // Если список пустой, возвращённый итератор будет равен end()
        // Результат вызова эквивалентен вызову метода cbegin()
        [[nodiscard]] ConstIterator begin() const noexcept
        {
            return ConstIterator{head_.next_node};
        }

        // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
        // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
        // Результат вызова эквивалентен вызову метода cend()
        [[nodiscard]] ConstIterator end() const noexcept
        {
            return ConstIterator{nullptr};
        }

        // Возвращает константный итератор, ссылающийся на первый элемент
        // Если список пустой, возвращённый итератор будет равен cend()
        [[nodiscard]] ConstIterator cbegin() const noexcept
        {
            return ConstIterator{head_.next_node};
        }

        // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
        // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
        [[nodiscard]] ConstIterator cend() const noexcept
        {
            return ConstIterator{nullptr};
        }

        // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
        // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
        [[nodiscard]] Iterator before_begin() noexcept
        {
            return Iterator{&head_};
        }

        // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
        // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
        [[nodiscard]] ConstIterator cbefore_begin() const noexcept
        {
            return ConstIterator{const_cast<Node*>(&head_)};
        }

        // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
        // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
        [[nodiscard]] ConstIterator before_begin() const noexcept
        {
            return ConstIterator{const_cast<Node*>(&head_)};
        }

        /*
         * Вставляет элемент value после элемента, на который указывает pos.
         * Возвращает итератор на вставленный элемент
         * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
         */
        Iterator InsertAfter(ConstIterator pos, const Type& value)
        {
            Node* new_node = new Node(value, pos.node_->next_node);
            pos.node_->next_node = new_node;
            ++size_;

            return Iterator{new_node};
        }

        /*
         * Удаляет элемент, следующий за pos.
         * Возвращает итератор на элемент, следующий за удалённым
         */
        Iterator EraseAfter(ConstIterator pos) noexcept
        {
            Node* node_for_del = pos.node_->next_node;
            pos.node_->next_node = pos.node_->next_node->next_node;
            delete node_for_del;

            return Iterator{pos.node_->next_node};
        }

        SingleLinkedList() : head_(), size_()
        {
        }

        SingleLinkedList(std::initializer_list<Type> values)
        {
            Clear();

            for(auto it = rbegin(values); it != rend(values); ++it)
            {
                PushFront(*it);
            }
        }

        SingleLinkedList(const SingleLinkedList& other)
        {
            SingleLinkedList tmp;

            Node* other_next_node = other.head_.next_node;

            if(other.head_.next_node != nullptr)
            {
                Node* first_node = new Node(other_next_node->value, nullptr);
                tmp.head_.next_node = first_node;

                other_next_node = other_next_node->next_node;
                Node* prev_node = first_node;

                while(other_next_node != nullptr)
                {
                    Node* new_node = new Node(other_next_node->value, nullptr);
                    prev_node->next_node = new_node;

                    prev_node = new_node;
                    other_next_node = other_next_node->next_node;
                }

                tmp.size_ = other.size_;
                swap(tmp);
            }
            else
            {
                swap(tmp);
            }
        }

        SingleLinkedList& operator=(const SingleLinkedList& rhs)
        {
            if(this == &rhs)
            {
                return *this;
            }

            SingleLinkedList tmp = rhs;

            swap(tmp);

            return *this;
        }

        // Обменивает содержимое списков за время O(1)
        void swap(SingleLinkedList& other) noexcept
        {
            std::swap(other.size_, size_);
            std::swap(other.head_.next_node, head_.next_node);
        }

        ~SingleLinkedList()
        {
            Clear();
        }

        // Возвращает количество элементов в списке за время O(1)
        [[nodiscard]] size_t GetSize() const noexcept
        {
            return size_;
        }

        // Сообщает, пустой ли список за время O(1)
        [[nodiscard]] bool IsEmpty() const noexcept
        {
            return size_ == 0 ? true : false;
        }

        void PushFront(const Type& value)
        {
            head_.next_node = new Node(value, head_.next_node);
            ++size_;
        }

        void PopFront() noexcept
        {
            if(head_.next_node != nullptr)
            {
                Node* node_for_del = head_.next_node;
                Node* node_after_del = head_.next_node->next_node;

                if(node_after_del != nullptr)
                {
                    head_.next_node = node_after_del;
                }
                else
                {
                    head_.next_node = nullptr;
                }

                delete node_for_del;
            }
        }

        void Clear()
        {
            if(head_.next_node != nullptr)
            {
                Node* first_node = head_.next_node;

                while(first_node != nullptr)
                {
                    Node* current_node = first_node;
                    first_node = first_node->next_node;
                    delete current_node;
                    size_--;
                }
            }

            head_.next_node = nullptr;
            size_ = 0;
        }

    private:
        // Фиктивный узел, используется для вставки "перед первым элементом"
        Node head_;
        size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept
{
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return !(lhs < rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return !(lhs < rhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return !(lhs < rhs);
}
