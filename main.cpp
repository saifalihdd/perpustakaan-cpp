#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

namespace Antrian
{
  // === QUEUE ===
  struct QueueNode
  {
    string nama;
    QueueNode *next;
    QueueNode(const string &n) : nama(n), next(nullptr) {}
  };

  struct Queue
  {
    QueueNode *front, *rear;

    Queue() : front(nullptr), rear(nullptr) {}

    void enqueue(const string &nama)
    {
      QueueNode *baru = new QueueNode(nama);
      if (!rear)
      {
        front = rear = baru;
      }
      else
      {
        rear->next = baru;
        rear = baru;
      }
    }

    string dequeue()
    {
      if (!front)
        return "";

      string nama = front->nama;
      QueueNode *temp = front;
      front = front->next;

      if (!front)
        rear = nullptr;
      delete temp;
      return nama;
    }

    bool empty() const
    {
      return front == nullptr;
    }

    void tampil() const
    {
      QueueNode *temp = front;
      while (temp)
      {
        cout << temp->nama << " ";
        temp = temp->next;
      }
    }
  };

} // namespace Antrian

namespace Buku
{
  // === STRUCT BUKU ===
  struct Book
  {
    int id;
    string judul;
    int halaman;
    bool isBorrowed;
    Antrian::Queue waitlist;

    Book() : id(0), judul(""), halaman(0), isBorrowed(false)
    {
    }
    Book(int j, string k, int l) : id(j), judul(k), halaman(l), isBorrowed(false) {}

    friend ostream &operator<<(ostream &COUT, const Book &b);
  };

  inline string getStatus(bool isBorrowed)
  {
    return isBorrowed ? "Dipinjam" : "Tersedia";
  }

  ostream &operator<<(ostream &COUT, const Book &b)
  {
    COUT << "ID buku: " << b.id << "\n"
         << "Judul buku: " << b.judul << "\n"
         << "Halaman: " << b.halaman << '\n'
         << "Status: " << getStatus(b.isBorrowed) << '\n';
    if (!b.waitlist.empty())
    {
      COUT << "Antrian: ";
      b.waitlist.tampil();
      COUT << '\n';
    }
    return COUT;
  }
}

// === STRUCT ANGGOTA / PEMINJAM ===
struct Anggota
{
  int id;
  string nama;
  Buku::Book pinjam;
  Anggota(int j, string k, Buku::Book b) : id(j), nama(k), pinjam(b) {}
};

struct DaftarPeminjam
{
  vector<Anggota> list;

  void tambah(const string &nama, const Buku::Book &buku)
  {
    Anggota a(list.size() + 1, nama, buku);
    list.push_back(a);
  }

  void tampilkan()
  {
    cout << "\n====== DAFTAR PEMINJAM ======\n";
    if (list.empty())
    {
      cout << "Belum ada peminjaman.\n";
    }
    else
    {
      for (const auto &a : list)
      {
        cout << "ID Anggota: " << a.id << "\n"
             << "Nama: " << a.nama << "\n"
             << "Buku: " << a.pinjam.judul << "\n\n";
      }
    }
  }

  void hapusByBookId(int idBuku)
  {
    for (auto it = list.begin(); it != list.end(); ++it)
    {
      if (it->pinjam.id == idBuku)
      {
        list.erase(it);
        break;
      }
    }
  }

  void savePeminjamToJson(const string &filename)
  {
    if (list.empty())
      return;

    json j = json::array();
    for (const auto &a : list)
    {
      json buku = {
          {"Id", a.pinjam.id},
          {"Judul", a.pinjam.judul},
          {"Halaman", a.pinjam.halaman},
          {"isBorrowed", a.pinjam.isBorrowed}};
      j.push_back({{"Id", a.id}, {"Nama", a.nama}, {"Buku", buku}});
    }

    ofstream out(filename);
    out << j.dump(4);
    out.close();
  }

  void loadPeminjam(const string &filename)
  {
    ifstream file(filename);
    if (!file.is_open())
    {
      cout << "File peminjam.json belum ada, membuat daftar baru...\n";
      return;
    }

    json j;
    try
    {
      file >> j;
    }
    catch (json::parse_error &e)
    {
      cout << "Gagal membaca isi file peminjam.json (kemungkinan kosong): " << e.what() << "\n";
      return;
    }

    for (const auto &a : j)
    {
      if (a.contains("Id") && a.contains("Nama") && a.contains("Buku"))
      {
        int id = a["Id"];
        string nama = a["Nama"];

        Buku::Book b;
        if (a["Buku"].contains("Id") && a["Buku"].contains("Judul") && a["Buku"].contains("Halaman") && a["Buku"].contains("isBorrowed"))
        {
          b.id = a["Buku"]["Id"];
          b.judul = a["Buku"]["Judul"];
          b.halaman = a["Buku"]["Halaman"];
          b.isBorrowed = a["Buku"]["isBorrowed"];
          list.push_back({id, nama, b});
        }
      }
    }

    file.close();
  }
};

// === TEMPLATE LINKED LIST UNTUK BUKU ===
template <typename T>
struct Node
{
  T b;
  Node *right;
  Node *left;
  Node(T value) : b(value), right(nullptr), left(nullptr) {}
};
// TREE NODE
struct Treenode
{
  int id;
  Node<Buku::Book> *bukuNode;
  Treenode *left;
  Treenode *right;

  Treenode(int i, Node<Buku::Book> *b) : id(i), bukuNode(b), left(nullptr), right(nullptr) {}
};
//=== BINARY TREE ===
struct Binarytree
{
  Treenode *root;
  Binarytree() : root(nullptr) {}
  void insert(Node<Buku::Book> *bukuNode)
  {
    root = insertItem(root, bukuNode);
  }
  Treenode *insertItem(Treenode *root, Node<Buku::Book> *bukuNode)
  {
    if (!root)
      return new Treenode(bukuNode->b.id, bukuNode);
    if (bukuNode->b.id < root->id)
    {
      root->left = insertItem(root->left, bukuNode);
    }
    else if (bukuNode->b.id > root->id)
    {
      root->right = insertItem(root->right, bukuNode);
    }
    return root;
  }

  Node<Buku::Book> *cariBuku(int id)
  {
    Treenode *hasil = cari(root, id);
    if (hasil)
      return hasil->bukuNode;

    return nullptr;
  }
  Treenode *cari(Treenode *root, int id)
  {
    if (!root || root->id == id)
      return root;

    if (id < root->id)
    {
      root = cari(root->left, id);
    }
    else if (id > root->id)
    {
      root = cari(root->right, id);
    }
    return root;
  }
};

template <typename T>
struct Linkedlist
{
  Node<T> *head;
  Linkedlist() : head(nullptr) {}

  void insert(const T &item)
  {
    Node<T> *newnode = new Node<T>(item);
    if (!head)
    {
      head = newnode;
      return;
    }
    head->left = newnode;
    newnode->right = head;
    head = newnode;
  }

  void insert(int id, string judul, int halaman)
  {
    T b(id, judul, halaman);
    insert(b);
  }

  void displayList()
  {
    Node<T> *temp = head;
    cout << "\n======= LIST BUKU ========\n";
    while (temp)
    {
      cout << temp->b << "\n";
      temp = temp->right;
    }
  }

  Node<Buku::Book> *cariBuku(int id)
  {
    Node<T> *temp = head;
    while (temp)
    {
      if (temp->b.id == id)
        return temp;
      temp = temp->right;
    }
    return nullptr;
  }

  void saveBukuKeJson(const string &filename)
  {
    json j = json::array();
    Node<T> *temp = head;
    while (temp)
    {
      json b = {
          {"id", temp->b.id},
          {"judul", temp->b.judul},
          {"halaman", temp->b.halaman},
          {"isBorrowed", temp->b.isBorrowed}};
      j.push_back(b);
      temp = temp->right;
    }
    ofstream outFile(filename);
    outFile << j.dump(4); // pretty print
    outFile.close();
  }

  bool isExists(int id)
  {
    return cariBuku(id) != nullptr;
  }
  void loadBuku(const string &filename, Binarytree &tree)
  {
    ifstream inFile(filename);
    if (!inFile)
    {
      cout << "File JSON tidak ditemukan. Membuat daftar baru...\n";
      return;
    }

    json j;
    inFile >> j;

    for (const auto &item : j)
    {
      Buku::Book b(
          item["id"],
          item["judul"],
          item["halaman"]);
      b.isBorrowed = item["isBorrowed"];
      insert(b);
      tree.insert(head);
    }
    inFile.close();
  }
};

// === CIRCULAR LINKED LIST UNTUK PETUGAS PERPUSTAKAAN ===
struct Petugas
{
  string nama;
  Petugas *next;
};

Petugas *tailPetugas = nullptr;

// Fungsi tambah petugas
void tambahPetugas(const string &nama)
{
  Petugas *baru = new Petugas{nama, nullptr};
  if (!tailPetugas)
  {
    tailPetugas = baru;
    tailPetugas->next = tailPetugas;
  }
  else
  {
    baru->next = tailPetugas->next;
    tailPetugas->next = baru;
    tailPetugas = baru;
  }
  cout << "Petugas '" << nama << "' berhasil ditambahkan.\n";
}

// Fungsi tampilkan semua petugas
void tampilPetugas()
{
  if (!tailPetugas)
  {
    cout << "Belum ada petugas yang ditambahkan.\n";
    return;
  }
  cout << "\nDaftar Petugas:\n";
  Petugas *temp = tailPetugas->next;
  do
  {
    cout << "- " << temp->nama << endl;
    temp = temp->next;
  } while (temp != tailPetugas->next);
}

// Fungsi simulasi rotasi petugas
void rotasiPetugas(int hari)
{
  if (!tailPetugas)
  {
    cout << "Tidak ada petugas untuk rotasi.\n";
    return;
  }

  Petugas *giliran = tailPetugas->next;
  cout << "\nSimulasi Rotasi Petugas Selama " << hari << " Hari:\n";
  for (int i = 1; i <= hari; ++i)
  {
    cout << "Hari ke-" << i << ": " << giliran->nama << " bertugas\n";
    giliran = giliran->next;
  }
}

void simpanPetugasKeJson(const string &filename)
{
  if (!tailPetugas)
    return;

  json j = json::array();
  Petugas *temp = tailPetugas->next;
  do
  {
    j.push_back({{"Nama", temp->nama}});
    temp = temp->next;
  } while (temp != tailPetugas->next);

  ofstream outFile(filename);
  outFile << j.dump(4);
  outFile.close();
}

void loadPetugas(const string &filename)
{
  ifstream file(filename);
  if (!file.is_open())
  {
    cout << "File petugas.json belum ada, membuat daftar baru...\n";
    return;
  }

  json j;
  try
  {
    file >> j;
  }
  catch (json::parse_error &e)
  {
    cout << "Gagal membaca isi file petugas.json (kemungkinan kosong): " << e.what() << "\n";
    return;
  }

  for (const auto &a : j)
  {
    if (a.contains("Nama"))
    {
      string nama = a["Nama"];
      tambahPetugas(nama);
    }
  }

  file.close();
}

// === STACK UNTUK RIWAYAT PEMINJAMAN ===
struct Riwayat
{
  string aksi;
  int idBuku;
  string nama;
  string judul;
  Riwayat *next;
  Riwayat(const string &a, int k, const string &n, const string &j) : aksi(a), idBuku(k), nama(n), judul(j), next(nullptr) {}
};

struct StackRiwayat
{
  Riwayat *top = nullptr;

  void push(const string &aksi, int idBuku, const string &nama, const string &judul)
  {
    Riwayat *baru = new Riwayat(aksi, idBuku, nama, judul);
    baru->next = top;
    top = baru;
  }

  void tampil()
  {
    cout << "\n======= RIWAYAT PEMINJAMAN =======\n";
    if (!top)
    {
      cout << "Belum ada riwayat.\n";
      return;
    }
    Riwayat *temp = top;
    while (temp)
    {
      cout << temp->nama << " " << temp->aksi << " buku '" << temp->judul << "'\n";
      temp = temp->next;
    }
  }

  void simpanRiwayat(const string &filename)
  {

    json j = json::array();
    Riwayat *temp = top;
    while (temp)
    {
      json b = {
          {"id", temp->idBuku},
          {"judul", temp->judul},
          {"Nama", temp->nama},
          {"Aksi", temp->aksi},
      };
      j.push_back(b);
      temp = temp->next;
    }
    ofstream file(filename);
    file << j.dump(4);
    file.close();
  }

  void LoadRiwayat(const string &filename)
  {
    ifstream file(filename);
    if (!file)
      return;

    json j;
    try
    {
      file >> j;
    }
    catch (json::parse_error &e)
    {
      cout << "Error loading riwayat: " << e.what() << "\n";
      return;
    }

    // Add array check
    if (!j.is_array())
      return;

    for (const auto &a : j)
    {
      // Check ALL required fields exist before accessing them
      if (a.contains("Nama") && a.contains("Id") &&
          a.contains("Judul") && a.contains("Aksi"))
      {
        int id = a["Id"];
        string judul = a["Judul"];
        string nama = a["Nama"];
        string aksi = a["Aksi"];
        // Push to history
        push(aksi, id, nama, judul);
      }
    }
    file.close();
  }
};
// === MAIN FUNCTION ===
int main()
{
  int pilihan, id, halaman;
  string judul, nama;

  Linkedlist<Buku::Book> list;
  DaftarPeminjam peminjam;
  Binarytree buku;
  StackRiwayat riwayat;

  list.loadBuku("books.json", buku);
  loadPetugas("petugas.json");
  peminjam.loadPeminjam("peminjam.json");
  riwayat.LoadRiwayat("riwayat.json");

  while (true)
  {
    cout << "\n======== PERPUSTAKAAN ========\n";
    cout << "1. Tambah buku\n"
         << "2. Pinjam buku\n"
         << "3. Kembalikan buku\n"
         << "4. Lihat buku\n"
         << "5. Lihat daftar peminjam\n"
         << "6. Cari buku\n"
         << "7. Tambah petugas\n"
         << "8. Simulasi rotasi petugas\n"
         << "9. Lihat riwayat peminjaman\n"
         << "10. Keluar\n";
    cout << "Pilih: ";
    cin >> pilihan;
    cin.ignore();

    switch (pilihan)
    {
    case 1:
      cout << "ID Buku: ";
      cin >> id;
      cin.ignore();
      cout << "Judul buku: ";
      getline(cin, judul);
      cout << "Jumlah halaman: ";
      cin >> halaman;
      if (!list.isExists(id))
      {
        list.insert(id, judul, halaman);
        Node<Buku::Book> *node = list.cariBuku(id);
        buku.insert(node);
        cout << "Buku ditambahkan.\n";
      }
      else
      {
        cout << "ID buku sudah ada.\n";
      }
      break;

    case 2:
    {
      cout << "ID Buku yang ingin dipinjam: ";
      cin >> id;
      cin.ignore();

      Node<Buku::Book> *node = list.cariBuku(id);
      if (node)
      {
        cout << "Nama peminjam: ";
        getline(cin, nama);
        if (!node->b.isBorrowed)
        {
          node->b.isBorrowed = true;
          peminjam.tambah(nama, node->b);
          riwayat.push("meminjam", node->b.id, nama, node->b.judul);
          cout << "Buku berhasil dipinjam oleh " << nama << ".\n";
        }
        else
        {
          cout << "Buku sedang dipinjam. Nama Anda dimasukkan ke antrian.\n";
          node->b.waitlist.enqueue(nama);
        }
      }
      else
      {
        cout << "Buku tidak ditemukan.\n";
      }
      break;
    }

    case 3:
    {
      cout << "ID Buku yang ingin dikembalikan: ";
      cin >> id;
      cin.ignore();

      Node<Buku::Book> *node = list.cariBuku(id);
      if (node && node->b.isBorrowed)
      {
        peminjam.hapusByBookId(id);
        if (!node->b.waitlist.empty())
        {
          string next = node->b.waitlist.dequeue();
          peminjam.tambah(next, node->b);
          riwayat.push("meminjam (dari antrian)", node->b.id, next, node->b.judul);
          cout << "Buku dikembalikan. Sekarang dipinjam oleh " << next << " dari antrian.\n";
        }
        else
        {
          node->b.isBorrowed = false;
          cout << "Buku berhasil dikembalikan dan tersedia kembali.\n";
        }
      }
      else
      {
        cout << "Buku tidak sedang dipinjam atau tidak ditemukan.\n";
      }
      break;
    }

    case 4:
      list.displayList();
      break;

    case 5:
      peminjam.tampilkan();
      break;
    case 6:
    {
      cout << "Masukkan Id buku: ";
      cin >> id;
      cin.ignore();
      Node<Buku::Book> *hasil = buku.cariBuku(id);
      if (hasil)
      {
        cout << "Buku berhasil ditemukan.\n";
        cout << hasil->b;
      }
      else
      {
        cout << "Buku tidak ditemukan.\n";
      }

      break;
    }
    case 7:
    {
      cout << "Masukkan nama petugas: ";
      getline(cin, nama);
      tambahPetugas(nama);
      break;
    }

    case 8:
    {
      tampilPetugas();
      int hari;
      cout << "Masukkan jumlah hari simulasi: ";
      cin >> hari;
      cin.ignore();
      rotasiPetugas(hari);
      break;
    }

    case 9:
      riwayat.tampil();
      break;

    case 10:
      cout << "Keluar dari program.\n";
      list.saveBukuKeJson("books.json");
      simpanPetugasKeJson("petugas.json");
      riwayat.simpanRiwayat("riwayat.json");
      peminjam.savePeminjamToJson("peminjam.json");
      return 0;
      break;

    default:
      cout << "Pilihan tidak valid.\n";
      break;
    }
  }

  return 0;
}
