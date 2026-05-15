#include <iostream>
#include <string>
using namespace std;

struct Petugas
{
  string nama;
  Petugas *next;
};

Petugas *tail = nullptr;

// Tambah petugas ke circular linked list
void tambahPetugas(string nama)
{
  Petugas *baru = new Petugas{nama, nullptr};
  if (tail == nullptr)
  {
    tail = baru;
    tail->next = tail;
  }
  else
  {
    baru->next = tail->next;
    tail->next = baru;
    tail = baru;
  }
  cout << "Petugas '" << nama << "' berhasil ditambahkan.\n";
}

// Tampilkan semua petugas
void tampilkanPetugas()
{
  if (tail == nullptr)
  {
    cout << "Belum ada petugas yang ditambahkan.\n";
    return;
  }

  cout << "Daftar Petugas:\n";
  Petugas *temp = tail->next;
  do
  {
    cout << "- " << temp->nama << endl;
    temp = temp->next;
  } while (temp != tail->next);
}

// Simulasi rotasi tugas
void rotasiPetugas(int hari)
{
  if (tail == nullptr)
  {
    cout << "Tidak ada petugas untuk dijadwalkan.\n";
    return;
  }

  Petugas *giliran = tail->next;
  for (int i = 1; i <= hari; ++i)
  {
    cout << "Hari ke-" << i << ": " << giliran->nama << " bertugas\n";
    giliran = giliran->next;
  }
}

int main()
{
  int pilihan;

  do
  {
    cout << "\n===== MENU ROTASI PETUGAS =====\n";
    cout << "1. Tambah Petugas\n";
    cout << "2. Simulasi Rotasi Tugas\n";
    cout << "0. Keluar\n";
    cout << "Pilih menu: ";
    cin >> pilihan;
    cin.ignore(); // Bersihkan newline

    switch (pilihan)
    {
    case 1:
    {
      string nama;
      cout << "Masukkan nama petugas: ";
      getline(cin, nama);
      tambahPetugas(nama);
      break;
    }
    case 2:
    {
      tampilkanPetugas();
      int hari;
      cout << "Masukkan jumlah hari simulasi: ";
      cin >> hari;
      cout << "\nSimulasi rotasi selama " << hari << " hari:\n";
      rotasiPetugas(hari);
      break;
    }
    case 0:
      cout << "Terima kasih! Program selesai.\n";
      break;
    default:
      cout << "Pilihan tidak valid. Coba lagi.\n";
    }

  } while (pilihan != 0);

  return 0;
}
