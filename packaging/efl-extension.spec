Name:       efl-extension
Summary:    EFL extension library
Version:    0.1.7
Release:    1
Group:      System/Libraries
License:    Flora-1.1
URL:        http://www.tizen.org/
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(fontconfig)
BuildRequires:  cmake
BuildRequires:  gettext
BuildRequires:  pkgconfig(cairo)
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig


%description
EFL extension library


%package devel
Summary:    EFL extensiona library (devel)
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}


%description devel
EFL extension library providing small utility functions (devel)


%prep
%setup -q


%build
export CFLAGS+=" -fvisibility=hidden -fPIC -Wall"
export LDFLAGS+=" -fvisibility=hidden -Wl,-z,defs -Wl,--hash-style=both -Wl,--as-needed"

cmake \
	. -DCMAKE_INSTALL_PREFIX=/usr

make %{?jobs:-j%jobs}


%install
%make_install

mkdir -p %{buildroot}/%{_datadir}/license
cp %{_builddir}/%{buildsubdir}/LICENSE %{buildroot}/%{_datadir}/license/%{name}


%post -p /sbin/ldconfig


%postun -p /sbin/ldconfig


%files
%defattr(-,root,root,-)
%{_libdir}/libefl-extension.so.*
%{_datadir}/locale/*
%{_datadir}/license/%{name}
%manifest %{name}.manifest
%{_datadir}/efl-extension/images/*

%files devel
%defattr(-,root,root,-)
%{_includedir}/efl-extension/*.h
%{_libdir}/*.so
%{_libdir}/pkgconfig/efl-extension.pc
%{_includedir}/efl-extension/circle/*.h
%{_includedir}/efl-extension/common/*.h